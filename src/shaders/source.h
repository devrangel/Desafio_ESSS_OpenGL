namespace ShaderSource
{
	static const char* vertexShaderSource = R"(#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec2 aTexCoord;

	out vec2 TexCoord;	

	void main()
	{
		gl_Position = vec4(aPos, 1.0f);
		TexCoord = aTexCoord;
	})";

	static const char* fragmentShaderSource = R"(#version 330 core
	in vec2 TexCoord;

	out vec4 FragColor;

	uniform sampler2D uTexture;

	void main()
	{
		FragColor = texture(uTexture, TexCoord);
	})";
};
