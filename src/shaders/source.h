namespace ShaderSource
{
	static const char* vertexShaderSource = R"(#version 330 core
	layout (location = 0) in vec3 aPos;

	uniform mat4 uModel;
	uniform mat4 uView;
	uniform mat4 uProjection;

	out float height;

	void main()
	{
		height = aPos.y;
		gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
	})";

	static const char* fragmentShaderSource = R"(#version 330 core
	in float height;	
	
	out vec4 FragColor;

	void main()
	{
		float hei = (height + 16) / 32.0f;
		FragColor = vec4(hei, hei, hei, 1.0f);
	})";
};
