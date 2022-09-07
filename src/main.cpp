#include <iostream>
#include <memory>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shaders/shader.h"
#include "shaders/source.h"
#include "utils.h"

void processInput(GLFWwindow* window);

/*
 * Callbacks
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};

	Shader shader(ShaderSource::vertexShaderSource, ShaderSource::fragmentShaderSource);

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shader.useProgram();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, 3);

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
}

/*
 * Callbacks
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
