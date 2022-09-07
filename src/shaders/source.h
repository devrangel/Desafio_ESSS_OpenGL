namespace ShaderSource
{
	static const char* vertexShaderSource = R"(#version 330 core
	layout (location = 0) in vec3 aPos;
	void main()
	{
		gl_Position = vec4(aPos, 1.0f);
	}
)";

	static const char* fragmentShaderSource = R"(#version 330 core
	out vec4 FragColor;
	void main()
	{
		FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	}
)";

};
