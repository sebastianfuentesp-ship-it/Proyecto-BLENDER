#pragma once

// Modelos animados hechos en Blender y exportados a glTF (.gltf + .bin + texturas), cargados con assimp.
// Sirve para las dos clases de animacion que salen de Blender:
//  - de huesos (esqueleto + pesos por vertice): se calcula la matriz de cada hueso y el vertex shader
//    (shaders/anim.vs) mueve cada vertice con la suma de sus huesos ("skinning").
//  - por objetos (keyframes de posicion, rotacion y escala de cada objeto): cada malla se dibuja con la
//    matriz de su nodo, que cambia con el tiempo (reja, mariposas, humo).

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"

GLint TextureFromFile(const char *path, std::string directory);   // de Model.h

const int MAX_HUESOS = 40;

class ModeloAnimado
{
public:
	// segundos: lo que dura la animacion en Blender (sirve para no depender de la unidad de tiempo del glTF)
	ModeloAnimado(const std::string &ruta, float segundos)
	{
		this->segundos = segundos;
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(ruta, aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_GenSmoothNormals);
		if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		this->directorio = ruta.substr(0, ruta.find_last_of('/'));
		this->CargaNodos(scene->mRootNode, -1);
		for (unsigned i = 0; i < scene->mNumMeshes; i++)
		{
			this->CargaMalla(scene->mMeshes[i], scene, i);
		}
		// Blender exporta una animacion por objeto animado (reja: una por hoja; mariposas: una por mariposa y ala),
		// asi que se juntan los canales de todas; todas empiezan y terminan juntas
		for (unsigned an = 0; an < scene->mNumAnimations; an++)
		{
			aiAnimation *a = scene->mAnimations[an];
			this->duracion = glm::max(this->duracion, (float)a->mDuration);
			for (unsigned c = 0; c < a->mNumChannels; c++)
			{
				aiNodeAnim *ch = a->mChannels[c];
				auto it = this->indiceNodo.find(ch->mNodeName.C_Str());
				if (it == this->indiceNodo.end())
				{
					continue;
				}
				Canal k;
				for (unsigned j = 0; j < ch->mNumPositionKeys; j++)
				{
					aiVector3D v = ch->mPositionKeys[j].mValue;
					k.tp.push_back((float)ch->mPositionKeys[j].mTime); k.pos.push_back(glm::vec3(v.x, v.y, v.z));
				}
				for (unsigned j = 0; j < ch->mNumRotationKeys; j++)
				{
					aiQuaternion q = ch->mRotationKeys[j].mValue;
					k.tr.push_back((float)ch->mRotationKeys[j].mTime); k.rot.push_back(glm::quat(q.w, q.x, q.y, q.z));
				}
				for (unsigned j = 0; j < ch->mNumScalingKeys; j++)
				{
					aiVector3D v = ch->mScalingKeys[j].mValue;
					k.te.push_back((float)ch->mScalingKeys[j].mTime); k.esc.push_back(glm::vec3(v.x, v.y, v.z));
				}
				this->nodos[it->second].canal = (int)this->canales.size();
				this->canales.push_back(k);
			}
		}
		this->global.assign(this->nodos.size(), glm::mat4(1.0f));
		this->huesos.assign(MAX_HUESOS, glm::mat4(1.0f));
		this->Pose(0.0f);
		std::cout << "Animacion " << ruta << ": " << this->mallas.size() << " mallas, " << this->offset.size() << " huesos, "
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
	std::map<std::string, int> indiceHueso;
	std::vector<int> huesoNodo;
	std::vector<glm::mat4> offset;
	std::vector<glm::mat4> huesos;
	std::vector<Canal> canales;
	std::map<std::string, GLuint> texturas;
	float duracion = 0.0f;
	float segundos = 1.0f;

	static glm::mat4 Mat(const aiMatrix4x4 &m)
	{
		return glm::transpose(glm::make_mat4(&m.a1));   // assimp guarda por renglones, glm por columnas
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

	void CargaNodos(aiNode *n, int padre)
	{
		int i = (int)this->nodos.size();
		this->nodos.push_back({ n->mName.C_Str(), Mat(n->mTransformation), padre, -1 });
		this->indiceNodo[n->mName.C_Str()] = i;
		for (unsigned c = 0; c < n->mNumChildren; c++)
		{
			this->CargaNodos(n->mChildren[c], i);
		}
	}

	int NodoDeMalla(aiNode *n, unsigned malla)
	{
		for (unsigned k = 0; k < n->mNumMeshes; k++)
		{
			if (n->mMeshes[k] == malla) return this->indiceNodo[n->mName.C_Str()];
		}
		for (unsigned c = 0; c < n->mNumChildren; c++)
		{
			int r = this->NodoDeMalla(n->mChildren[c], malla);
			if (r >= 0) return r;
		}
		return -1;
	}

	void CargaMalla(aiMesh *mesh, const aiScene *scene, unsigned indice)
	{
		std::vector<Vertice> vs(mesh->mNumVertices);
		for (unsigned i = 0; i < mesh->mNumVertices; i++)
		{
			Vertice &v = vs[i];
			v.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			v.Normal = mesh->mNormals ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0, 1, 0);
			v.TexCoords = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
			for (int k = 0; k < 4; k++)
			{
				v.ids[k] = 0; v.pesos[k] = 0.0f;
			}
		}
		// pesos de los huesos: cada vertice guarda hasta 4 huesos
		for (unsigned b = 0; b < mesh->mNumBones; b++)
		{
			aiBone *bone = mesh->mBones[b];
			std::string nombre = bone->mName.C_Str();
			if (this->indiceHueso.find(nombre) == this->indiceHueso.end())
			{
				this->indiceHueso[nombre] = (int)this->offset.size();
				this->offset.push_back(Mat(bone->mOffsetMatrix));
				this->huesoNodo.push_back(this->indiceNodo.count(nombre) ? this->indiceNodo[nombre] : 0);
			}
			int id = this->indiceHueso[nombre];
			for (unsigned w = 0; w < bone->mNumWeights; w++)
			{
				Vertice &v = vs[bone->mWeights[w].mVertexId];
				for (int k = 0; k < 4; k++)
				{
					if (v.pesos[k] == 0.0f)
					{
						v.ids[k] = id; v.pesos[k] = bone->mWeights[w].mWeight;
						break;
					}
				}
			}
		}
		std::vector<GLuint> idx;
		for (unsigned f = 0; f < mesh->mNumFaces; f++)
		{
			for (unsigned k = 0; k < mesh->mFaces[f].mNumIndices; k++)
			{
				idx.push_back(mesh->mFaces[f].mIndices[k]);
			}
		}

		Malla m;
		m.n = (GLsizei)idx.size();
		m.piel = mesh->mNumBones > 0;
		m.nodo = this->NodoDeMalla(scene->mRootNode, indice);
		if (m.nodo < 0) m.nodo = 0;
		m.textura = 0;
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		aiString tex;
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 && mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex) == AI_SUCCESS)
		{
			std::string t = tex.C_Str();
			if (this->texturas.find(t) == this->texturas.end())
			{
				this->texturas[t] = TextureFromFile(t.c_str(), this->directorio);
			}
			m.textura = this->texturas[t];
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
