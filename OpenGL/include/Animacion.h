#pragma once

// Modelos animados hechos en Blender. Blender los exporta a glTF (.gltf + .bin + texturas) y
// herramientas/gltf_a_anim.py los pasa a un .anim de texto que se lee aqui sin assimp (el assimp del proyecto
// es viejo y no lee el glTF de Blender 5). Sirve para las dos clases de animacion que salen de Blender:
//  - de huesos (esqueleto + pesos por vertice): se calcula la matriz de cada hueso y el vertex shader
//    (shaders/anim.vs) mueve cada vertice con la suma de sus huesos ("skinning").
//  - por objetos (keyframes de posicion, rotacion y escala de cada objeto): cada malla se dibuja con la
//    matriz de su nodo, que cambia con el tiempo (reja, mariposas, humo).
//
// Formato del .anim (todo en texto, matrices de 16 numeros por columnas como glm):
//   nodos N      / nodo nombre padre matriz_local
//   huesos B     / hueso nodo matriz_offset
//   mallas M     / malla nodo piel textura nVertices nIndices, luego un renglon por vertice
//                  (x y z  nx ny nz  u v  4 huesos  4 pesos) y un renglon por triangulo
//   duracion d
//   canales C    / canal nodo, luego pos n, rot n (w x y z) y esc n con un renglon "tiempo valores" por llave

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

GLint TextureFromFile(const char *path, std::string directory);   // de Model.h

const int MAX_HUESOS = 40;

class ModeloAnimado
{
public:
	// ruta: el .gltf o el .anim (se lee el .anim de la misma carpeta)
	// segundos: lo que dura la animacion en Blender
	ModeloAnimado(const std::string &ruta, float segundos)
	{
		this->segundos = segundos;
		std::string archivo = ruta.substr(0, ruta.find_last_of('.')) + ".anim";
		this->directorio = ruta.substr(0, ruta.find_last_of('/'));
		std::ifstream f(archivo);
		std::string palabra;
		int n = 0;
		if (!f || !(f >> palabra) || palabra != "ANIM")
		{
			std::cout << "ERROR::ANIMACION:: no se pudo leer " << archivo << std::endl;
			return;
		}
		f >> palabra;   // version
		f >> palabra >> n;   // nodos
		for (int i = 0; i < n; i++)
		{
			Nodo nd;
			f >> palabra >> nd.nombre >> nd.padre;
			LeeMat(f, nd.local);
			nd.canal = -1;
			this->indiceNodo[nd.nombre] = i;
			this->nodos.push_back(nd);
		}
		f >> palabra >> n;   // huesos
		for (int b = 0; b < n; b++)
		{
			int nodo;
			glm::mat4 m;
			f >> palabra >> nodo;
			LeeMat(f, m);
			this->huesoNodo.push_back(nodo);
			this->offset.push_back(m);
		}
		f >> palabra >> n;   // mallas
		for (int i = 0; i < n; i++)
		{
			this->CargaMalla(f);
		}
		f >> palabra >> this->duracion;
		f >> palabra >> n;   // canales
		for (int c = 0; c < n; c++)
		{
			int nodo, k;
			Canal ch;
			f >> palabra >> nodo;
			f >> palabra >> k;
			for (int j = 0; j < k; j++)
			{
				glm::vec3 v; float t;
				f >> t >> v.x >> v.y >> v.z;
				ch.tp.push_back(t); ch.pos.push_back(v);
			}
			f >> palabra >> k;
			for (int j = 0; j < k; j++)
			{
				glm::quat q; float t;
				f >> t >> q.w >> q.x >> q.y >> q.z;
				ch.tr.push_back(t); ch.rot.push_back(q);
			}
			f >> palabra >> k;
			for (int j = 0; j < k; j++)
			{
				glm::vec3 v; float t;
				f >> t >> v.x >> v.y >> v.z;
				ch.te.push_back(t); ch.esc.push_back(v);
			}
			if (nodo >= 0 && nodo < (int)this->nodos.size())
			{
				this->nodos[nodo].canal = (int)this->canales.size();
				this->canales.push_back(ch);
			}
		}
		this->global.assign(this->nodos.size(), glm::mat4(1.0f));
		this->huesos.assign(MAX_HUESOS, glm::mat4(1.0f));
		this->Pose(0.0f);
		std::cout << "Animacion " << archivo << ": " << this->mallas.size() << " mallas, " << this->offset.size() << " huesos, "
			<< this->canales.size() << " canales" << std::endl;
	}

	float Segundos() const { return this->segundos; }

	// Calcula la pose en el tiempo t (segundos desde el inicio de la animacion, se recorta al final)
	void Pose(float t)
	{
		t = glm::clamp(t, 0.0f, this->segundos);
		float ticks = (this->segundos > 0.0f) ? t / this->segundos * this->duracion : 0.0f;
		for (size_t i = 0; i < this->nodos.size(); i++)
		{
			Nodo &n = this->nodos[i];
			glm::mat4 local = n.local;
			if (n.canal >= 0)
			{
				Canal &k = this->canales[n.canal];
				glm::vec3 p = k.pos.empty() ? glm::vec3(n.local[3]) : Interpola(k.tp, k.pos, ticks);
				glm::quat r = k.rot.empty() ? glm::quat_cast(glm::mat3(n.local)) : InterpolaQ(k.tr, k.rot, ticks);
				glm::vec3 e = k.esc.empty() ? glm::vec3(1.0f) : Interpola(k.te, k.esc, ticks);
				local = glm::translate(glm::mat4(1.0f), p) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), e);
			}
			this->global[i] = (n.padre >= 0) ? this->global[n.padre] * local : local;   // los padres van antes que los hijos
		}
		for (size_t b = 0; b < this->offset.size() && b < (size_t)MAX_HUESOS; b++)
		{
			this->huesos[b] = this->global[this->huesoNodo[b]] * this->offset[b];
		}
	}

	// Dibuja con el shader de animacion (anim.vs + lighting.frag); model coloca el modelo en la escena
	void Draw(Shader &shader, const glm::mat4 &model)
	{
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
		GLint pielLoc = glGetUniformLocation(shader.Program, "piel");
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "huesos"), MAX_HUESOS, GL_FALSE, glm::value_ptr(this->huesos[0]));
		for (auto &m : this->mallas)
		{
			glm::mat4 mm = m.piel ? model : model * this->global[m.nodo];
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mm));
			glUniform1i(pielLoc, m.piel ? 1 : 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m.textura);
			glBindVertexArray(m.VAO);
			glDrawElements(GL_TRIANGLES, m.n, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
		glUniform1i(pielLoc, 0);
	}

	// Posicion en el mundo (sin el model de la escena) del origen de un nodo, p. ej. una mariposa
	glm::vec3 PosicionNodo(const std::string &nombre)
	{
		auto it = this->indiceNodo.find(nombre);
		return it == this->indiceNodo.end() ? glm::vec3(0.0f) : glm::vec3(this->global[it->second][3]);
	}

private:
	struct Vertice
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		GLint ids[4];
		GLfloat pesos[4];
	};
	struct Malla
	{
		GLuint VAO, VBO, EBO;
		GLsizei n;
		GLuint textura;
		int nodo;
		bool piel;
	};
	struct Nodo
	{
		std::string nombre;
		glm::mat4 local;
		int padre;
		int canal;
	};
	struct Canal
	{
		std::vector<float> tp, tr, te;
		std::vector<glm::vec3> pos, esc;
		std::vector<glm::quat> rot;
	};

	std::string directorio;
	std::vector<Nodo> nodos;
	std::map<std::string, int> indiceNodo;
	std::vector<glm::mat4> global;
	std::vector<Malla> mallas;
	std::vector<int> huesoNodo;
	std::vector<glm::mat4> offset;
	std::vector<glm::mat4> huesos;
	std::vector<Canal> canales;
	std::map<std::string, GLuint> texturas;
	float duracion = 0.0f;
	float segundos = 1.0f;

	static void LeeMat(std::ifstream &f, glm::mat4 &m)
	{
		float v[16];
		for (int i = 0; i < 16; i++)
		{
			f >> v[i];
		}
		m = glm::make_mat4(v);   // el .anim ya viene por columnas, como glm
	}

	static int Llave(const std::vector<float> &t, float x)
	{
		int i = 0;
		while (i + 2 < (int)t.size() && t[i + 1] <= x)
		{
			i++;
		}
		return i;
	}

	static glm::vec3 Interpola(const std::vector<float> &t, const std::vector<glm::vec3> &v, float x)
	{
		if (v.size() == 1) return v[0];
		int i = Llave(t, x);
		float f = glm::clamp((x - t[i]) / glm::max(t[i + 1] - t[i], 1e-6f), 0.0f, 1.0f);
		return glm::mix(v[i], v[i + 1], f);
	}

	static glm::quat InterpolaQ(const std::vector<float> &t, const std::vector<glm::quat> &v, float x)
	{
		if (v.size() == 1) return v[0];
		int i = Llave(t, x);
		float f = glm::clamp((x - t[i]) / glm::max(t[i + 1] - t[i], 1e-6f), 0.0f, 1.0f);
		return glm::normalize(glm::slerp(v[i], v[i + 1], f));
	}

	void CargaMalla(std::ifstream &f)
	{
		std::string palabra, tex;
		Malla m;
		int nv, ni, piel;
		f >> palabra >> m.nodo >> piel >> tex >> nv >> ni;
		m.piel = piel == 1;
		std::vector<Vertice> vs(nv);
		for (auto &v : vs)
		{
			f >> v.Position.x >> v.Position.y >> v.Position.z >> v.Normal.x >> v.Normal.y >> v.Normal.z
				>> v.TexCoords.x >> v.TexCoords.y >> v.ids[0] >> v.ids[1] >> v.ids[2] >> v.ids[3]
				>> v.pesos[0] >> v.pesos[1] >> v.pesos[2] >> v.pesos[3];
		}
		std::vector<GLuint> idx(ni);
		for (auto &i : idx)
		{
			f >> i;
		}
		m.n = (GLsizei)idx.size();
		if (m.nodo < 0 || m.nodo >= (int)this->nodos.size()) m.nodo = 0;
		m.textura = 0;
		if (tex != "-")
		{
			if (this->texturas.find(tex) == this->texturas.end())
			{
				this->texturas[tex] = TextureFromFile(tex.c_str(), this->directorio);
			}
			m.textura = this->texturas[tex];
		}

		glGenVertexArrays(1, &m.VAO);
		glGenBuffers(1, &m.VBO);
		glGenBuffers(1, &m.EBO);
		glBindVertexArray(m.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
		glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(Vertice), vs.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (GLvoid *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (GLvoid *)offsetof(Vertice, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (GLvoid *)offsetof(Vertice, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertice), (GLvoid *)offsetof(Vertice, ids));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertice), (GLvoid *)offsetof(Vertice, pesos));
		glBindVertexArray(0);
		this->mallas.push_back(m);
	}
};
