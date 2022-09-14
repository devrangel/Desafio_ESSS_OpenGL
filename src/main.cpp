#include <iostream>
#include <memory>
#include <windows.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "shaders/shader.h"
#include "shaders/source.h"
#include "camera.h"
#include "utils.h"

/*
 * Forces nvidia's driver to be used
 */
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void processInput(GLFWwindow* window);

/*
 * Callbacks
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/*
 * Screen size
 */
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

/*
 * Initial values for Camera
 */
Camera camera(
	glm::vec3(200.0f, 500.0f, 200.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	SCREEN_WIDTH / 2.0f,
	SCREEN_HEIGHT / 2.0f,
	-150.0f,
	-40.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// Inicializa o GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // APPLE

	// Cria uma janela com o contexto do OpenGL e define o contexto como current
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Desafio ESSS OpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		// TODO: add spdlog message
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Register callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeLimits(window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Inicializa o GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// TODO: add spdlog message
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shader(
		ShaderSource::vertexShaderSource,
		ShaderSource::fragmentShaderSource,
		ShaderSource::tesselletionControlShaderSource,
		ShaderSource::tesselletionEvaluationShaderSource);

	/*
	 * Creating a grid based with the loaded heightmap
	 * Each pixel corresponds to a unit of measurement in the grid
	 * For example: an image of 800 x 600 has 800 row and 600 columns
	 */
	int width = 0;
	int height = 0;
	int nrChannels = 0;

	/*
	 * Buffer for Heightmap Texture
	 */
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int rez = 20;
	std::vector<float> vertices;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("textures/heightmap.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		shader.setUniformInt("heightMap", 0);

		/*
		 * Vertex Generation
		 */
		float heightInMin = -height / 2.0f;
		float widthInMin = -width / 2.0f;

		for (unsigned int i = 0; i < rez - 1; i++)
		{
			for (unsigned int j = 0; j < rez - 1; j++)
			{
				vertices.push_back(widthInMin + width * i / (float)rez);
				vertices.push_back(0.0f);
				vertices.push_back(heightInMin + height * j / (float)rez);
				vertices.push_back(i / (float)rez);
				vertices.push_back(j / (float)rez);

				vertices.push_back(widthInMin + width * (i + 1) / (float)rez);
				vertices.push_back(0.0f);
				vertices.push_back(heightInMin + height * j / (float)rez);
				vertices.push_back((i + 1) / (float)rez);
				vertices.push_back(j / (float)rez);

				vertices.push_back(widthInMin + width * i / (float)rez);
				vertices.push_back(0.0f);
				vertices.push_back(heightInMin + height * (j + 1) / (float)rez);
				vertices.push_back(i / (float)rez);
				vertices.push_back((j + 1) / (float)rez);

				vertices.push_back(widthInMin + width * (i + 1) / (float)rez);
				vertices.push_back(0.0f);
				vertices.push_back(heightInMin + height * (j + 1) / (float)rez);
				vertices.push_back((i + 1) / (float)rez);
				vertices.push_back((j + 1) / (float)rez);
			}
		}
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	/*
	 * VAO
	 */
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/*
	 * VBO
	 */
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// TexCoord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	shader.useProgram();

	/*
	 * Model, View, Projection Matrix
	 */
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	int modelLocation = glGetUniformLocation(shader.getId(), "uModel");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	/*
	 * glEnable
	 */
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		/*
		 * Calculate the time each frame took to render
		 * This value is used to keep a smooth camera moviment
		 */
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.m_Zoom), (float)SCREEN_WIDTH / float(SCREEN_HEIGHT), 0.1f, 100000.0f);
		int projectionLocation = glGetUniformLocation(shader.getId(), "uProjection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glm::mat4 viewMatrix = camera.getViewMatrix();
		int viewLocaltion = glGetUniformLocation(shader.getId(), "uView");
		glUniformMatrix4fv(viewLocaltion, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	camera.processKeyboard(window);
}

/*
 * Callbacks
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.mouseCallback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	camera.mouseButtonCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.scrollCallback(window, xoffset, yoffset);
}
