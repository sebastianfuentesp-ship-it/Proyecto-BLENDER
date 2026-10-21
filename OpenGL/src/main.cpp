// Proyecto final: Panteon en noche de Dia de Muertos
// Basado en el codigo del laboratorio (fuentesDeLuz.cpp): Shader, Camera y Model del profesor.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Animacion.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void AvanzaAnimaciones(float dt);
GLuint LoadCubemap(const std::vector<std::string> &faces);

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera: entrada del panteon, afuera de la reja, mirando hacia la capilla
Camera  camera(glm::vec3(0.0f, 1.7f, 19.5f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Interacciones con las luces
bool linternaOn = false;     // F: linterna (spotlight en la camara)
bool veladorasOn = true;     // V: velas (luces puntuales)
bool fresnelOn = true;       // R: brillo de luna en los bordes (Fresnel)
bool nieblaOn = true;        // N: niebla
float lunaIntensidad = 1.0f; // + / -: intensidad de la luna

// Animaciones (teclas 1 a 9)
// De huesos, hechas en Blender (glTF): 1 persona caminando, 2 senora que se hinca a rezar, 3 nino que abraza una tumba
// Por objetos, hechas en Blender (glTF): 4 reja de la entrada, 5 mariposas monarca, 6 humo del copal
// Por codigo (jerarquia y maquina de estados, como el perro del laboratorio): 7 banda, 8 nino que saluda, 9 viento
bool caminaOn = false;  float tCamina = 0.0f;
bool rezoOn = false;    float tRezo = 0.0f;
bool abrazoOn = false;  float tAbrazo = 0.0f;
bool rejaAbierta = false; float tReja = 0.0f;
bool mariposasOn = false; float tMariposas = 0.0f;
bool humoOn = false;    float tHumo = 0.0f;
bool bandaOn = false;   float tBanda = 0.0f;  float fuerzaBanda = 0.0f;
bool saludoOn = false;
bool vientoOn = false;  float fuerzaViento = 0.0f;

// Persona que camina por el camino: va y viene entre la entrada y el altar
const float CAMINO_Z0 = 12.8f, CAMINO_Z1 = 3.2f, CAMINO_X = 0.62f;
const float VELOCIDAD = 1.2f;            // metros por segundo (un ciclo de pasos dura 1 s)
float caminaZ = CAMINO_Z0;
float caminaRot = 180.0f;                // mirando hacia el altar (-Z)
int caminaDir = -1;                      // -1 hacia el altar, +1 hacia la entrada
float caminaGiro = 0.0f;                 // segundos que lleva girando en una punta

// Senora rezando y nino abrazando la tumba: se hincan, se quedan en la parte de en medio mientras la tecla
// este activa y se levantan al apagarla (los tiempos son los de los keyframes de Blender)
const float REZO_LOOP0 = 3.5f, REZO_LOOP1 = 6.25f, REZO_FIN = 8.0f;
const float ABRAZO_LOOP0 = 3.25f, ABRAZO_LOOP1 = 6.75f, ABRAZO_FIN = 8.0f;

// Nino que saluda: maquina de estados
enum EstadoSaludo { QUIETO, SUBE_BRAZO, SALUDA, BAJA_BRAZO };
EstadoSaludo estadoSaludo = QUIETO;
float brazoSaludo = 0.0f;    // grados que sube el brazo derecho (0 = abajo, 150 = arriba)
float manoSaludo = 0.0f;     // grados que se mueve el antebrazo de lado a lado
bool manoDerecha = true;
float cabezaSaludo = 0.0f;

// Parte de un modelo jerarquico (una parte por OBJ con el origen en su articulacion), leido de pivotes.txt
struct Parte
{
	std::string nombre;
	Model *modelo;
	glm::vec3 pivote;     // en el modelo completo
	std::string padre;
};

// Escena: una linea por objeto en assets/escena_layout.txt
struct Instancia
{
	std::string modelo;
	glm::vec3 pos;
	float rotY;
	float escala;
	glm::vec3 tinte;   // color que se multiplica por la textura (tumbas pintadas, velas de color)
};

// Lo que se dibuja de cada instancia: matriz ya calculada, esfera envolvente en el mundo y version ligera (LOD)
struct Dibujo
{
	Model *modelo;
	Model *lod;          // assets/models/<m>/<m>_lod.obj si existe (se usa de lejos)
	glm::mat4 model;
	glm::vec3 centro;    // centro de la esfera envolvente en el mundo
	float radio;
	glm::vec3 tinte;
};

// Optimizacion: solo se dibuja lo que esta dentro de la vista, lo chico y lejano se oculta en la niebla
// y lo que esta lejos se dibuja con su version ligera.
const float DIST_LOD = 11.0f;      // metros: a partir de aqui se usa el LOD
const float DIST_NIEBLA = 40.0f;   // con niebla, lo chico (radio < 1.5 m) mas lejos que esto no se ve
const float DIST_SIN_NIEBLA = 90.0f;

// Velas: cada instancia de "vela" del layout lleva su flama y es una luz puntual candidata.
// El shader recibe solo las MAX_LUCES velas mas cercanas a la camara.
const int MAX_LUCES = 24;
const float FLAMA_BASE = 0.30f;   // altura donde empieza la flama en el modelo de la vela
std::vector<glm::mat4> velas;     // matriz de modelo de cada vela
std::vector<glm::vec3> luzVela;   // posicion de cada flama en el mundo

// Cubo para el skybox
float vertices[] = {
	-0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f
};

// Niebla y cielo
const glm::vec3 FOG_COLOR(0.33f, 0.31f, 0.39f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

std::vector<Instancia> CargaLayout(const std::string &ruta)
{
	std::vector<Instancia> v;
	std::ifstream f(ruta);
	std::string linea;

	if (!f.is_open())
	{
		std::cout << "No se encontro " << ruta << std::endl;
	}

	while (std::getline(f, linea))
	{
		if (linea.empty() || linea[0] == '#')
		{
			continue;
		}

		std::istringstream ss(linea);
		Instancia i;
		ss >> i.modelo >> i.pos.x >> i.pos.y >> i.pos.z >> i.rotY >> i.escala;
		if (!(ss >> i.tinte.r >> i.tinte.g >> i.tinte.b))
		{
			i.tinte = glm::vec3(1.0f);   // sin tinte
		}
		v.push_back(i);
	}

	return v;
}

// Matriz de modelo de una instancia: T * Ry * S
glm::mat4 MatrizModelo(const Instancia &i)
{
	glm::mat4 model(1);
	model = glm::translate(model, i.pos);
	model = glm::rotate(model, glm::radians(i.rotY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(i.escala));
	return model;
}

// Parpadeo de la flama: suma de senos con fases distintas para cada vela
float Parpadeo(int i, float t)
{
	return 0.80f + 0.12f * sin(t * 7.3f + i * 1.7f) + 0.08f * sin(t * 13.1f + i * 4.1f);
}

// Lee pivotes.txt de un modelo por partes y carga cada parte
std::vector<Parte> CargaPartes(const std::string &carpeta)
{
	std::vector<Parte> partes;
	std::ifstream f(carpeta + "/pivotes.txt");
	std::string linea;
	while (std::getline(f, linea))
	{
		if (linea.empty() || linea[0] == '#')
		{
			continue;
		}
		std::istringstream ss(linea);
		Parte p;
		ss >> p.nombre >> p.pivote.x >> p.pivote.y >> p.pivote.z >> p.padre;
		std::string ruta = carpeta + "/" + p.nombre + ".obj";
		p.modelo = new Model((char*)ruta.c_str());
		partes.push_back(p);
	}
	return partes;
}

glm::vec3 PivoteDe(const std::vector<Parte> &partes, const std::string &nombre)
{
	for (auto &p : partes)
	{
		if (p.nombre == nombre)
		{
			return p.pivote;
		}
	}
	return glm::vec3(0.0f);
}

// Dibuja un modelo por partes: cada parte = matriz de su padre * T(offset desde el padre) * R(su angulo)
// giros: nombre de la parte -> (angulo x, angulo y, angulo z) en grados; mueve: traslacion extra de una parte
std::map<std::string, glm::mat4> extraPartes;   // matriz extra de alguna parte (el brazo del trombon), en su articulacion

void DibujaPartes(std::vector<Parte> &partes, const glm::mat4 &base, std::map<std::string, glm::vec3> &giros,
	std::map<std::string, glm::vec3> &mueve, Shader &shader, GLint modelLoc)
{
	std::map<std::string, glm::mat4> mats;
	for (auto &p : partes)   // en pivotes.txt los padres van antes que sus hijos
	{
		bool tienePadre = mats.count(p.padre) > 0;
		glm::mat4 m = tienePadre ? mats[p.padre] : base;
		glm::vec3 offset = tienePadre ? p.pivote - PivoteDe(partes, p.padre) : p.pivote;
		m = glm::translate(m, offset + (mueve.count(p.nombre) ? mueve[p.nombre] : glm::vec3(0.0f)));
		if (giros.count(p.nombre))
		{
			glm::vec3 g = giros[p.nombre];
			m = glm::rotate(m, glm::radians(g.y), glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::rotate(m, glm::radians(g.x), glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::rotate(m, glm::radians(g.z), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (extraPartes.count(p.nombre))
		{
			m = m * extraPartes[p.nombre];
		}
		mats[p.nombre] = m;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		p.modelo->Draw(shader);
	}
}

// Animacion de la banda por codigo. Cada musico: el cuerpo se mece y rebota con el ritmo, el torso se inclina,
// los de percusion golpean con los brazos y el trombon mueve la vara. k (0 a 1) suaviza el arranque y el final.
void PoseMusico(const std::string &id, int i, float t, float k, std::map<std::string, glm::vec3> &giros,
	std::map<std::string, glm::vec3> &mueve, glm::vec3 dirVara)
{
	const float PI2 = 6.2831853f;
	float pulso = t * 2.0f;                       // 120 golpes por minuto
	float vaiven = sin(PI2 * 0.5f * pulso + i * 0.7f);
	giros["cuerpo"] = glm::vec3(0.0f, 6.0f * k * vaiven, 0.0f);
	giros["torso"] = glm::vec3(-4.0f * k * fabs(sin(PI2 * 0.5f * pulso)), 0.0f, 5.0f * k * vaiven);
	mueve["cuerpo"] = glm::vec3(0.0f, 0.02f * k * fabs(sin(PI2 * 0.5f * pulso + i)), 0.0f);
	if (id == "tarola" || id == "tambora")
	{
		float golpeDer = glm::max(0.0f, sin(PI2 * pulso));
		float golpeIzq = glm::max(0.0f, sin(PI2 * pulso + 3.1416f));
		if (id == "tambora")
		{
			golpeIzq = 0.5f * glm::max(0.0f, sin(PI2 * 0.5f * pulso));   // el platillo cada dos tiempos
		}
		giros["brazoder"] = glm::vec3(-35.0f * k * golpeDer, 0.0f, 0.0f);
		giros["brazoizq"] = glm::vec3(-35.0f * k * golpeIzq, 0.0f, 0.0f);
	}
	else if (id == "trombon")
	{
		float v = 0.5f - 0.5f * cos(PI2 * 0.25f * pulso);
		float sale = 0.32f * k * v;                  // la vara sale hasta 0.4 m sin despegarse
		mueve["vara"] = dirVara * sale;
		// la mano derecha va agarrada de la vara: el brazo gira desde el hombro hacia donde quedo la mano
		// y se estira lo que haga falta (h: del hombro a la mano en reposo)
		glm::vec3 h(0.153f, -0.019f, 0.419f);
		glm::vec3 h2 = h + dirVara * sale;
		glm::vec3 a = glm::normalize(h), b = glm::normalize(h2);
		float ang = acos(glm::clamp(glm::dot(a, b), -1.0f, 1.0f));
		glm::vec3 eje = glm::cross(a, b);
		glm::mat4 r(1.0f);
		if (glm::length(eje) > 1e-5f)
		{
			r = glm::rotate(glm::mat4(1.0f), ang, glm::normalize(eje));
		}
		float estira = glm::length(h2) / glm::length(h);
		glm::mat3 e = glm::mat3(1.0f) + (estira - 1.0f) * glm::outerProduct(a, a);
		extraPartes["brazoder"] = r * glm::mat4(e);
	}
	else
	{
		// trompetas, clarinete y tuba: suben un poco el instrumento al tocar
		giros["torso"].x += -6.0f * k * (0.5f + 0.5f * sin(PI2 * 0.25f * pulso + i));
	}
}

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Panteon - Dia de Muertos", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("shaders/lighting.vs", "shaders/lighting.frag");
	Shader lampShader("shaders/lamp.vs", "shaders/lamp.frag");
	Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.frag");
	Shader animShader("shaders/anim.vs", "shaders/lighting.frag");   // misma iluminacion, con huesos

	// Layout de la escena y modelos: cada modelo se carga una sola vez
	std::vector<Instancia> layout = CargaLayout("assets/escena_layout.txt");
	std::map<std::string, Model*> modelos;
	std::map<std::string, Model*> lods;

	for (auto &i : layout)
	{
		if (modelos.find(i.modelo) == modelos.end())
		{
			std::string ruta = "assets/models/" + i.modelo + "/" + i.modelo + ".obj";
			std::cout << "Cargando " << ruta << std::endl;
			modelos[i.modelo] = new Model((char*)ruta.c_str());
			std::string rutaLod = "assets/models/" + i.modelo + "/" + i.modelo + "_lod.obj";
			lods[i.modelo] = std::ifstream(rutaLod).good() ? new Model((char*)rutaLod.c_str()) : nullptr;
		}
	}

	// Se calcula una vez la matriz y la esfera de cada instancia; se ordenan por modelo para cambiar menos de texturas
	std::vector<Dibujo> dibujos;
	for (auto &i : layout)
	{
		if (i.modelo == "papel_picado")
		{
			continue;   // se dibuja aparte (viento)
		}
		Dibujo d;
		d.modelo = modelos[i.modelo];
		d.lod = lods[i.modelo];
		d.model = MatrizModelo(i);
		d.centro = glm::vec3(d.model * glm::vec4(d.modelo->Centro(), 1.0f));
		d.radio = d.modelo->Radio() * i.escala;
		d.tinte = i.tinte;
		dibujos.push_back(d);
	}
	std::stable_sort(dibujos.begin(), dibujos.end(), [](const Dibujo &a, const Dibujo &b) { return a.modelo < b.modelo; });

	// Flama de las velas: se dibuja sin iluminacion con el shader de lampara
	Model flama((char*)"assets/models/flama/flama.obj");

	for (auto &i : layout)
	{
		if (i.modelo == "vela")
		{
			glm::mat4 m = MatrizModelo(i);
			velas.push_back(m);
			luzVela.push_back(glm::vec3(m * glm::vec4(0.0f, FLAMA_BASE + 0.03f, 0.0f, 1.0f)));
		}
	}
	std::cout << velas.size() << " velas en la escena" << std::endl;

	// ---------------- animaciones ----------------
	ModeloAnimado caminar("assets/models/animaciones/anim_caminar/anim_caminar.gltf", 1.0f);
	ModeloAnimado rezar("assets/models/animaciones/anim_rezar/anim_rezar.gltf", 8.0f);
	ModeloAnimado abrazar("assets/models/animaciones/anim_abrazar/anim_abrazar.gltf", 8.0f);
	ModeloAnimado reja("assets/models/animaciones/reja_animada/reja_animada.gltf", 4.0f);
	ModeloAnimado mariposas("assets/models/animaciones/mariposas/mariposas.gltf", 8.0f);
	ModeloAnimado humo("assets/models/animaciones/humo_copal/humo_copal.gltf", 4.0f);

	// Donde quedan la senora que reza y el nino que abraza la tumba (Escena/animaciones_layout.txt)
	glm::mat4 lugarRezo(1.0f), lugarAbrazo(1.0f), lugarSaludo(1.0f), lugarBanda(1.0f);
	{
		std::ifstream f("assets/animaciones_layout.txt");
		std::string linea;
		while (std::getline(f, linea))
		{
			if (linea.empty() || linea[0] == '#')
			{
				continue;
			}
			std::istringstream ss(linea);
			std::string nombre;
			glm::vec3 p;
			float r;
			ss >> nombre >> p.x >> p.y >> p.z >> r;
			glm::mat4 m = glm::rotate(glm::translate(glm::mat4(1.0f), p), glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
			if (nombre == "anim_rezar") lugarRezo = m;
			if (nombre == "anim_abrazar") lugarAbrazo = m;
			if (nombre == "nino_saluda") lugarSaludo = m;
			if (nombre == "banda") lugarBanda = m;
		}
	}

	// Papel picado: se dibuja con el shader de animacion para que el viento mueva las tiras
	Model *papelPicado = nullptr;
	std::vector<glm::mat4> papeles;
	for (auto &i : layout)
	{
		if (i.modelo == "papel_picado")
		{
			papeles.push_back(MatrizModelo(i));
			papelPicado = modelos[i.modelo];
		}
	}

	// Nino que saluda (modelo por partes, como el perro de la practica)
	std::vector<Parte> ninoSaluda = CargaPartes("assets/models/persona_nino_partes");

	// Banda: siete musicos por partes, acomodados en arco (assets/models/banda/banda_layout.txt)
	struct Musico
	{
		std::string id;
		std::vector<Parte> partes;
		glm::mat4 lugar;
	};
	std::vector<Musico> banda;
	glm::vec3 dirVara(0.0f, -0.309f, 0.951f);
	{
		std::ifstream f("assets/models/banda/banda_layout.txt");
		std::string linea;
		while (std::getline(f, linea))
		{
			if (linea.empty() || linea[0] == '#')
			{
				continue;
			}
			std::istringstream ss(linea);
			Musico m;
			glm::vec3 p;
			float r;
			ss >> m.id >> p.x >> p.y >> p.z >> r;
			m.partes = CargaPartes("assets/models/banda/musico_" + m.id);
			m.lugar = glm::rotate(glm::translate(lugarBanda, p), glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
			banda.push_back(m);
		}
	}
	std::vector<int> orden(luzVela.size());

	// VAO del cubo (skybox)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Skybox (orden de GL_TEXTURE_CUBE_MAP_POSITIVE_X + i)
	std::vector<std::string> caras = {
		"assets/skybox/right.png", "assets/skybox/left.png",
		"assets/skybox/top.png",   "assets/skybox/bottom.png",
		"assets/skybox/front.png", "assets/skybox/back.png"
	};
	GLuint cubemapTexture = LoadCubemap(caras);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 0); // usa la misma textura para el brillo
	animShader.Use();
	glUniform1i(glGetUniformLocation(animShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(animShader.Program, "material.specular"), 0);
	skyboxShader.Use();
	glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);

	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 200.0f);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		AvanzaAnimaciones(deltaTime);

		// Las luces se le pasan igual al shader normal y al de animacion
		glm::vec3 cam = camera.GetPosition();
		auto ponLuces = [&](GLuint prog)
		{
			glUniform3f(glGetUniformLocation(prog, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

			// Directional light: luz de luna azulada
			glm::vec3 luna = glm::vec3(0.45f, 0.50f, 0.75f) * lunaIntensidad;
			glUniform3f(glGetUniformLocation(prog, "dirLight.direction"), -0.4f, -0.8f, 0.45f);
			glUniform3f(glGetUniformLocation(prog, "dirLight.ambient"), 0.12f * lunaIntensidad, 0.12f * lunaIntensidad, 0.18f * lunaIntensidad);
			glUniform3f(glGetUniformLocation(prog, "dirLight.diffuse"), luna.x, luna.y, luna.z);
			glUniform3f(glGetUniformLocation(prog, "dirLight.specular"), 0.15f * lunaIntensidad, 0.15f * lunaIntensidad, 0.2f * lunaIntensidad);

			// Point lights: las velas mas cercanas a la camara, con luz calida que parpadea
			for (int i = 0; i < (int)orden.size(); i++)
			{
				orden[i] = i;
			}
			int nLuces = std::min(MAX_LUCES, (int)orden.size());
				std::partial_sort(orden.begin(), orden.begin() + nLuces, orden.end(), [&](int a, int b)
			{
				return glm::length(luzVela[a] - cam) < glm::length(luzVela[b] - cam);
			});

			for (int i = 0; i < MAX_LUCES; i++)
			{
				std::string base = "pointLights[" + std::to_string(i) + "].";
				int v = (i < nLuces) ? orden[i] : 0;
				float k = (veladorasOn && i < nLuces) ? Parpadeo(v, currentFrame) : 0.0f;
				glm::vec3 c = glm::vec3(1.0f, 0.55f, 0.2f) * k;
				glm::vec3 p = (i < nLuces) ? luzVela[v] : glm::vec3(0.0f);
				glUniform3f(glGetUniformLocation(prog, (base + "position").c_str()), p.x, p.y, p.z);
				glUniform3f(glGetUniformLocation(prog, (base + "ambient").c_str()), c.x * 0.05f, c.y * 0.05f, c.z * 0.05f);
				glUniform3f(glGetUniformLocation(prog, (base + "diffuse").c_str()), c.x, c.y, c.z);
				glUniform3f(glGetUniformLocation(prog, (base + "specular").c_str()), c.x * 0.3f, c.y * 0.3f, c.z * 0.3f);
				glUniform1f(glGetUniformLocation(prog, (base + "constant").c_str()), 1.0f);
				glUniform1f(glGetUniformLocation(prog, (base + "linear").c_str()), 0.7f);
				glUniform1f(glGetUniformLocation(prog, (base + "quadratic").c_str()), 1.8f);
			}

			// SpotLight: linterna en la camara
			float sl = linternaOn ? 1.0f : 0.0f;
			glUniform3f(glGetUniformLocation(prog, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
			glUniform3f(glGetUniformLocation(prog, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
			glUniform3f(glGetUniformLocation(prog, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(prog, "spotLight.diffuse"), 1.0f * sl, 0.95f * sl, 0.8f * sl);
			glUniform3f(glGetUniformLocation(prog, "spotLight.specular"), 0.5f * sl, 0.5f * sl, 0.5f * sl);
			glUniform1f(glGetUniformLocation(prog, "spotLight.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(prog, "spotLight.linear"), 0.09f);
			glUniform1f(glGetUniformLocation(prog, "spotLight.quadratic"), 0.032f);
			glUniform1f(glGetUniformLocation(prog, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
			glUniform1f(glGetUniformLocation(prog, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

			// Fresnel
			glUniform3f(glGetUniformLocation(prog, "fresnelColor"), 0.55f, 0.65f, 1.0f);
			glUniform1f(glGetUniformLocation(prog, "fresnelStrength"), fresnelOn ? 0.35f * lunaIntensidad : 0.0f);
			glUniform1f(glGetUniformLocation(prog, "fresnelPower"), 4.0f);

			// Niebla
			glUniform1i(glGetUniformLocation(prog, "fogOn"), nieblaOn ? 1 : 0);
			glUniform3f(glGetUniformLocation(prog, "fogColor"), FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z);
			glUniform1f(glGetUniformLocation(prog, "fogDensity"), 0.085f);
			glUniform1f(glGetUniformLocation(prog, "fogHeight"), 3.0f);
		};

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLuint prog = lightingShader.Program;
		ponLuces(prog);

		// Create camera transformations
		glm::mat4 view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(prog, "model");
		GLint viewLoc = glGetUniformLocation(prog, "view");
		GLint projLoc = glGetUniformLocation(prog, "projection");
		GLint tinteLoc = glGetUniformLocation(prog, "tinte");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Planos de la vista (frustum) para descartar lo que queda fuera de la pantalla
		glm::mat4 pv = projection * view;
		glm::vec4 planos[6];
		for (int k = 0; k < 3; k++)
		{
			planos[2 * k] = glm::vec4(pv[0][3] + pv[0][k], pv[1][3] + pv[1][k], pv[2][3] + pv[2][k], pv[3][3] + pv[3][k]);
			planos[2 * k + 1] = glm::vec4(pv[0][3] - pv[0][k], pv[1][3] - pv[1][k], pv[2][3] - pv[2][k], pv[3][3] - pv[3][k]);
		}
		for (auto &p : planos)
		{
			p /= glm::length(glm::vec3(p));
		}
		float distMax = nieblaOn ? DIST_NIEBLA : DIST_SIN_NIEBLA;

		// Dibuja cada instancia del layout: model = T * Ry * S
		for (auto &d : dibujos)
		{
			bool visible = true;
			for (auto &p : planos)
			{
				if (glm::dot(glm::vec3(p), d.centro) + p.w < -d.radio)
				{
					visible = false;
					break;
				}
			}
			if (!visible)
			{
				continue;
			}

			float dist = glm::length(d.centro - cam) - d.radio;
			if (d.radio < 1.5f && dist > distMax)
			{
				continue;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(d.model));
			glUniform3f(tinteLoc, d.tinte.r, d.tinte.g, d.tinte.b);
			Model *m = (d.lod != nullptr && dist > DIST_LOD) ? d.lod : d.modelo;
			m->Draw(lightingShader);
		}
		glBindVertexArray(0);

		// ---------------- animaciones por codigo (modelos por partes) ----------------
		glUniform3f(tinteLoc, 1.0f, 1.0f, 1.0f);
		{
			// nino que saluda: el brazo derecho sube de lado y el antebrazo va y viene
			std::map<std::string, glm::vec3> giros, mueve;
			giros["brazoder"] = glm::vec3(0.0f, 0.0f, -brazoSaludo);
			giros["antebrazoder"] = glm::vec3(0.0f, 0.0f, manoSaludo - 0.25f * brazoSaludo);
			giros["cabeza"] = glm::vec3(0.0f, 0.0f, cabezaSaludo);
			DibujaPartes(ninoSaluda, lugarSaludo, giros, mueve, lightingShader, modelLoc);
		}
		for (int i = 0; i < (int)banda.size(); i++)
		{
			std::map<std::string, glm::vec3> giros, mueve;
			extraPartes.clear();
			PoseMusico(banda[i].id, i, tBanda, fuerzaBanda, giros, mueve, dirVara);
			DibujaPartes(banda[i].partes, banda[i].lugar, giros, mueve, lightingShader, modelLoc);
		}
		extraPartes.clear();
		glBindVertexArray(0);

		// ---------------- animaciones de Blender (huesos y objetos) y papel picado con viento ----------------
		animShader.Use();
		ponLuces(animShader.Program);
		glUniformMatrix4fv(glGetUniformLocation(animShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(animShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(animShader.Program, "tinte"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(animShader.Program, "viento"), 0.0f);
		glUniform1f(glGetUniformLocation(animShader.Program, "tiempo"), currentFrame);
		{
			// persona caminando: el ciclo de pasos solo avanza si camina; gira en las puntas
			glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(CAMINO_X, 0.0f, caminaZ));
			m = glm::rotate(m, glm::radians(caminaRot), glm::vec3(0.0f, 1.0f, 0.0f));
			caminar.Pose(caminaGiro > 0.0f ? 0.0f : fmod(tCamina, 1.0f));
			caminar.Draw(animShader, m);
			rezar.Pose(tRezo);
			rezar.Draw(animShader, lugarRezo);
			abrazar.Pose(tAbrazo);
			abrazar.Draw(animShader, lugarAbrazo);
			reja.Pose(tReja);
			reja.Draw(animShader, glm::mat4(1.0f));
			if (mariposasOn)
			{
				mariposas.Pose(fmod(tMariposas, mariposas.Segundos()));
				mariposas.Draw(animShader, glm::mat4(1.0f));
			}
			if (humoOn)
			{
				// humo transparente: se mezcla con lo que ya esta dibujado atras
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glUniform1i(glGetUniformLocation(animShader.Program, "suave"), 1);
				humo.Pose(fmod(tHumo, humo.Segundos()));
				humo.Draw(animShader, glm::mat4(1.0f));
				glUniform1i(glGetUniformLocation(animShader.Program, "suave"), 0);
				glDisable(GL_BLEND);
			}
		}
		// papel picado: el vertex shader mueve las tiras con el viento (las puntas de los postes no se mueven)
		glUniform1i(glGetUniformLocation(animShader.Program, "piel"), 0);
		glUniform1f(glGetUniformLocation(animShader.Program, "viento"), fuerzaViento);
		for (int i = 0; papelPicado && i < (int)papeles.size(); i++)
		{
			glUniform1f(glGetUniformLocation(animShader.Program, "fase"), i * 1.3f);
			glUniformMatrix4fv(glGetUniformLocation(animShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(papeles[i]));
			papelPicado->Draw(animShader);
		}
		glUniform1f(glGetUniformLocation(animShader.Program, "viento"), 0.0f);
		glBindVertexArray(0);

		// Flamas de las velas: se estiran y encogen con el parpadeo (si las velas estan apagadas no se dibujan)
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		GLint lampColorLoc = glGetUniformLocation(lampShader.Program, "lampColor");
		for (int i = 0; veladorasOn && i < (int)velas.size(); i++)
		{
			if (glm::length(luzVela[i] - cam) > distMax)
			{
				continue;
			}
			float k = Parpadeo(i, currentFrame);
			glm::mat4 model = velas[i];
			model = glm::translate(model, glm::vec3(0.0f, FLAMA_BASE, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 0.6f + 0.6f * k, 1.0f));
			model = glm::translate(model, glm::vec3(0.0f, -FLAMA_BASE, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(lampColorLoc, 1.0f, 0.55f + 0.3f * k, 0.15f + 0.2f * k);
			flama.Draw(lampShader);
		}
		glBindVertexArray(VAO);

		// Skybox al final: profundidad 1.0, sin la traslacion de la camara
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		glm::mat4 viewSky = glm::mat4(glm::mat3(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewSky));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(skyboxShader.Program, "fogColor"), FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z);
		glUniform1f(glGetUniformLocation(skyboxShader.Program, "fogHorizon"), nieblaOn ? 0.42f : -0.05f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	for (auto &m : modelos)
	{
		delete m.second;
	}
	for (auto &m : lods)
	{
		delete m.second;
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

// Carga las 6 caras del skybox en un GL_TEXTURE_CUBE_MAP
GLuint LoadCubemap(const std::vector<std::string> &faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (GLuint i = 0; i < faces.size(); i++)
	{
		int width, height;
		unsigned char *image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		if (image)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		else
		{
			std::cout << "No se pudo cargar " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// Avanza el tiempo de cada animacion segun su estado (se llama una vez por cuadro)
void AvanzaAnimaciones(float dt)
{
	// 1: persona caminando de ida y vuelta por el camino
	if (caminaOn)
	{
		if (caminaGiro > 0.0f)
		{
			// en la punta da media vuelta en 0.8 s
			caminaGiro += dt;
			caminaRot += 180.0f * dt / 0.8f;
			if (caminaGiro >= 0.8f)
			{
				caminaGiro = 0.0f;
				caminaDir = -caminaDir;
				caminaRot = (caminaDir < 0) ? 180.0f : 0.0f;
			}
		}
		else
		{
			tCamina += dt;
			caminaZ += caminaDir * VELOCIDAD * dt;
			if ((caminaDir < 0 && caminaZ <= CAMINO_Z1) || (caminaDir > 0 && caminaZ >= CAMINO_Z0))
			{
				caminaZ = glm::clamp(caminaZ, CAMINO_Z1, CAMINO_Z0);
				caminaGiro = 0.0001f;
			}
		}
	}

	// 2 y 3: se hincan, se quedan rezando / abrazando mientras este activa y se levantan al apagarla
	auto hincarse = [dt](bool on, float &t, float loop0, float loop1, float fin)
	{
		if (on)
		{
			t += dt;
			if (t > loop1)
			{
				t = loop0 + fmod(t - loop0, loop1 - loop0);
			}
		}
		else if (t > 0.0f)
		{
			if (t < loop1)
			{
				t = (t < loop0) ? glm::max(0.0f, t - 2.0f * dt) : loop1;   // si no termino de hincarse, se regresa
			}
			else
			{
				t += dt;
				if (t >= fin)
				{
					t = 0.0f;
				}
			}
		}
	};
	hincarse(rezoOn, tRezo, REZO_LOOP0, REZO_LOOP1, REZO_FIN);
	hincarse(abrazoOn, tAbrazo, ABRAZO_LOOP0, ABRAZO_LOOP1, ABRAZO_FIN);

	// 4: la reja se abre o se cierra (la misma animacion hacia adelante o hacia atras)
	tReja = glm::clamp(tReja + (rejaAbierta ? dt : -dt), 0.0f, 4.0f);

	// 5 y 6: mariposas y humo se repiten mientras esten activos
	if (mariposasOn) tMariposas += dt;
	if (humoOn) tHumo += dt;

	// 7: banda; la fuerza sube y baja en medio segundo para que no arranquen ni paren de golpe
	fuerzaBanda = glm::clamp(fuerzaBanda + (bandaOn ? 2.0f : -2.0f) * dt, 0.0f, 1.0f);
	if (fuerzaBanda > 0.0f) tBanda += dt;

	// 8: nino que saluda (maquina de estados)
	switch (estadoSaludo)
	{
	case QUIETO:
		if (saludoOn) estadoSaludo = SUBE_BRAZO;
		break;
	case SUBE_BRAZO:
		brazoSaludo += 150.0f * dt;
		cabezaSaludo = glm::min(cabezaSaludo + 20.0f * dt, 8.0f);
		if (brazoSaludo >= 150.0f)
		{
			brazoSaludo = 150.0f;
			estadoSaludo = SALUDA;
		}
		break;
	case SALUDA:
		manoSaludo += (manoDerecha ? 120.0f : -120.0f) * dt;
		if (manoSaludo > 25.0f) manoDerecha = false;
		if (manoSaludo < -25.0f) manoDerecha = true;
		if (!saludoOn && fabs(manoSaludo) < 4.0f)
		{
			manoSaludo = 0.0f;
			estadoSaludo = BAJA_BRAZO;
		}
		break;
	case BAJA_BRAZO:
		brazoSaludo -= 120.0f * dt;
		cabezaSaludo = glm::max(cabezaSaludo - 20.0f * dt, 0.0f);
		if (brazoSaludo <= 0.0f)
		{
			brazoSaludo = 0.0f;
			estadoSaludo = QUIETO;
		}
		break;
	}

	// 9: viento en el papel picado
	fuerzaViento = glm::clamp(fuerzaViento + (vientoOn ? 0.7f : -0.7f) * dt, 0.0f, 1.0f);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	// Intensidad de la luna
	if (keys[GLFW_KEY_EQUAL] || keys[GLFW_KEY_KP_ADD])
	{
		lunaIntensidad = glm::min(lunaIntensidad + 0.5f * deltaTime, 2.0f);
	}

	if (keys[GLFW_KEY_MINUS] || keys[GLFW_KEY_KP_SUBTRACT])
	{
		lunaIntensidad = glm::max(lunaIntensidad - 0.5f * deltaTime, 0.0f);
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_F)
		{
			linternaOn = !linternaOn;
		}

		if (key == GLFW_KEY_V)
		{
			veladorasOn = !veladorasOn;
		}

		if (key == GLFW_KEY_R)
		{
			fresnelOn = !fresnelOn;
		}

		if (key == GLFW_KEY_N)
		{
			nieblaOn = !nieblaOn;
		}

		// Animaciones
		if (key == GLFW_KEY_1) caminaOn = !caminaOn;
		if (key == GLFW_KEY_2) rezoOn = !rezoOn;
		if (key == GLFW_KEY_3) abrazoOn = !abrazoOn;
		if (key == GLFW_KEY_4) rejaAbierta = !rejaAbierta;
		if (key == GLFW_KEY_5) mariposasOn = !mariposasOn;
		if (key == GLFW_KEY_6) humoOn = !humoOn;
		if (key == GLFW_KEY_7) bandaOn = !bandaOn;
		if (key == GLFW_KEY_8) saludoOn = !saludoOn;
		if (key == GLFW_KEY_9) vientoOn = !vientoOn;
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
