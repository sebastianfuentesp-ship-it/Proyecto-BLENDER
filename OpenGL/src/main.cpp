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

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
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

// Escena: una linea por objeto en assets/escena_layout.txt
struct Instancia
{
	std::string modelo;
	glm::vec3 pos;
	float rotY;
	float escala;
	glm::vec3 tinte;   // color que se multiplica por la textura (tumbas pintadas, velas de color)
};

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

	// Layout de la escena y modelos: cada modelo se carga una sola vez
	std::vector<Instancia> layout = CargaLayout("assets/escena_layout.txt");
	std::map<std::string, Model*> modelos;

	for (auto &i : layout)
	{
		if (modelos.find(i.modelo) == modelos.end())
		{
			std::string ruta = "assets/models/" + i.modelo + "/" + i.modelo + ".obj";
			std::cout << "Cargando " << ruta << std::endl;
			modelos[i.modelo] = new Model((char*)ruta.c_str());
		}
	}

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

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLuint prog = lightingShader.Program;

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
		glm::vec3 cam = camera.GetPosition();
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

		// Dibuja cada instancia del layout: model = T * Ry * S
		for (auto &i : layout)
		{
			glm::mat4 model = MatrizModelo(i);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(tinteLoc, i.tinte.r, i.tinte.g, i.tinte.b);
			modelos[i.modelo]->Draw(lightingShader);
		}

		// Flamas de las velas: se estiran y encogen con el parpadeo (si las velas estan apagadas no se dibujan)
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		for (int i = 0; veladorasOn && i < (int)velas.size(); i++)
		{
			float k = Parpadeo(i, currentFrame);
			glm::mat4 model = velas[i];
			model = glm::translate(model, glm::vec3(0.0f, FLAMA_BASE, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 0.6f + 0.6f * k, 1.0f));
			model = glm::translate(model, glm::vec3(0.0f, -FLAMA_BASE, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(glGetUniformLocation(lampShader.Program, "lampColor"), 1.0f, 0.55f + 0.3f * k, 0.15f + 0.2f * k);
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
