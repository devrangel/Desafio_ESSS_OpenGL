#include "shader.h"

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, &vertexSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, &fragmentSource);

	this->m_Id = createProgram(vertexShader, fragmentShader);
}

GLuint Shader::getId() const
{
	return this->m_Id;
}

void Shader::useProgram()
{
	glUseProgram(this->m_Id);
}

void Shader::setUniformBool(const std::string& name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(this->m_Id, name.c_str()), (GLint)value);
}

void Shader::setUniformInt(const std::string& name, GLint value) const
{
	glUniform1i(glGetUniformLocation(this->m_Id, name.c_str()), value);
}

void Shader::setUniformFloat(const std::string& name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(this->m_Id, name.c_str()), value);
}

GLuint Shader::compileShader(GLenum shaderType, const GLchar* const* shaderSourceCode) const
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, shaderSourceCode, nullptr);
	glCompileShader(shader);

	GLint success;
	GLchar infoLog[512];

	if (shaderType == GL_VERTEX_SHADER)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
			return -1;
		}
	}
	else if (shaderType == GL_FRAGMENT_SHADER)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
			return -1;
		}
	}

	return shader;
}

GLuint Shader::createProgram(GLuint vertexShader, GLuint fragmentShader) const
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}