#include <iostream>
#include <memory>

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
#include "utils.h"

void processInput(GLFWwindow* window);

/*
 * Callbacks
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// Inicializa o GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // APPLE

	// Cria uma janela com o contexto do OpenGL e define o contexto como current
	GLFWwindow* window = glfwCreateWindow(800, 600, "Desafio ESSS OpenGL", nullptr, nullptr);
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

	// Inicializa o GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// TODO: add spdlog message
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*
	 * Rendering stuff
	 */
	float vertices[] = {
		// Position			// Texture Coords
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // top left
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	/*
	 * VAO e VBO
	 */
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Indices
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/*
	 * Texture
	 */
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	
	unsigned char* data = stbi_load("textures/heightmap.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	Shader shader(ShaderSource::vertexShaderSource, ShaderSource::fragmentShaderSource);

	shader.useProgram();
	shader.setUniformInt("uTexture", 0);

	/*
	 * Model, View, Projection Matrix
	 */
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	int modelLocation = glGetUniformLocation(shader.getId(), "uModel");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	int projectionLocation = glGetUniformLocation(shader.getId(), "uProjection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	/*
	 * glEnable
	 */
	glEnable(GL_DEPTH_TEST);

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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		int viewLocaltion = glGetUniformLocation(shader.getId(), "uView");
		glUniformMatrix4fv(viewLocaltion, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

/*
 * Callbacks
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
