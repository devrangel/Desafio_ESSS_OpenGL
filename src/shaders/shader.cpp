#include "shader.h"

Shader::Shader(
	const char* vertexSource,
	const char* fragmentSource,
	const char* tessControlShaderSource,
	const char* tessEvaluationShaderSource)
{
	this->m_Id = glCreateProgram();

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, &vertexSource);
	glAttachShader(this->m_Id, vertexShader);

	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, &fragmentSource);
	glAttachShader(this->m_Id, fragmentShader);

	GLuint tessControlShader;
	if (tessControlShaderSource != nullptr)
	{
		tessControlShader = compileShader(GL_TESS_CONTROL_SHADER, &tessControlShaderSource);
		glAttachShader(this->m_Id, tessControlShader);
	}

	GLuint tessEvaluationShader;
	if (tessEvaluationShaderSource != nullptr)
	{
		tessEvaluationShader = compileShader(GL_TESS_EVALUATION_SHADER, &tessEvaluationShaderSource);
		glAttachShader(this->m_Id, tessEvaluationShader);
	}

	glLinkProgram(this->m_Id);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(this->m_Id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->m_Id, sizeof(infoLog), nullptr, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvaluationShader);
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

	if (shaderType == GL_VERTEX_SHADER)
	{
		this->checkCompileErros(shader, "VERTEX");
	}
	else if (shaderType == GL_FRAGMENT_SHADER)
	{
		this->checkCompileErros(shader, "FRAGMENT");
	}
	else if (shaderType == GL_TESS_CONTROL_SHADER)
	{
		this->checkCompileErros(shader, "TESSELLETION_CONTROL");
	}
	else if (shaderType == GL_TESS_EVALUATION_SHADER)
	{
		this->checkCompileErros(shader, "TESSELLETION_EVALUATION");
	}

	return shader;
}

GLvoid Shader::checkCompileErros(GLuint shader, const std::string name) const
{
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		std::cout << "ERROR::SHADER::" << name << "::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
	}
}