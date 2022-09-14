#pragma once

#include <string>
#include <iostream>

#include "glad/glad.h"

class Shader
{
public:
	Shader(
		const char* vertexSource,
		const char* fragmentSource,
		const char* tessControlShaderSource = nullptr,
		const char* tessEvaluationShaderSource = nullptr);

	GLuint getId() const;
	void useProgram();

	void setUniformBool(const std::string& name, GLboolean value) const;
	void setUniformInt(const std::string& name, GLint value) const;
	void setUniformFloat(const std::string& name, GLfloat value) const;

private:
	GLuint m_Id;

	GLuint compileShader(GLenum shaderType, const GLchar* const* shaderSourceCode) const;
	GLvoid checkCompileErros(GLuint shader, const std::string name) const;
};
