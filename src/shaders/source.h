namespace ShaderSource
{
	static const char* vertexShaderSource = R"(#version 410 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec2 aTex;

	out vec2 TexCoord;

	void main()
	{
		gl_Position = vec4(aPos, 1.0f);
		TexCoord = aTex;
	})";

	static const char* tesselletionControlShaderSource = R"(#version 410 core
	layout (vertices = 4) out;

	uniform mat4 uModel;
	uniform mat4 uView;
	
	in vec2 TexCoord[];

	out vec2 TextureCoord[];

	void main()
	{
		gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
		TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

		if (gl_InvocationID == 0)
		{
			const int MIN_TESS_LEVEL = 4;
			const int MAX_TESS_LEVEL = 64;
			const float MIN_DISTANCE = 20;
			const float MAX_DISTANCE = 800;

			// each vertex to eye space
			vec4 eyeSpacePos00 = uView * uModel * gl_in[0].gl_Position;
			vec4 eyeSpacePos01 = uView * uModel * gl_in[1].gl_Position;
			vec4 eyeSpacePos10 = uView * uModel * gl_in[2].gl_Position;
			vec4 eyeSpacePos11 = uView * uModel * gl_in[3].gl_Position;

			float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
			float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
			float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
			float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

			float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
			float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
			float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
			float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

			gl_TessLevelOuter[0] = tessLevel0;
			gl_TessLevelOuter[1] = tessLevel1;
			gl_TessLevelOuter[2] = tessLevel2;
			gl_TessLevelOuter[3] = tessLevel3;
			
			gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
			gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
		}
	})";

	static const char* tesselletionEvaluationShaderSource = R"(#version 410 core
	layout (quads, fractional_odd_spacing, ccw) in;

	uniform sampler2D heightMap;
	uniform mat4 uModel;
	uniform mat4 uView;
	uniform mat4 uProjection;
	
	in vec2 TextureCoord[];

	out float Height;

	void main()
	{
		float u = gl_TessCoord.x;
		float v = gl_TessCoord.y;

		vec2 t00 = TextureCoord[0];
		vec2 t01 = TextureCoord[1];
		vec2 t10 = TextureCoord[2];
		vec2 t11 = TextureCoord[3];

		vec2 t0 = (t01 - t00) * u + t00;
		vec2 t1 = (t11 - t10) * u + t10;
		vec2 texCoord = (t1 - t0) * v + t0;

		Height = texture(heightMap, texCoord).y * 64.0 - 16.0;

		vec4 p00 = gl_in[0].gl_Position;
		vec4 p01 = gl_in[1].gl_Position;
		vec4 p10 = gl_in[2].gl_Position;
		vec4 p11 = gl_in[3].gl_Position;

		vec4 uVec = p01 - p00;
		vec4 vVec = p10 - p00;
		vec4 normal = normalize(vec4(cross(vVec.xyz, uVec.xyz), 0));

		vec4 p0 = (p01 - p00) * u + p00;
		vec4 p1 = (p11 - p10) * u + p10;
		vec4 p = (p1 - p0) * v + p0;

		p += normal * Height;

		gl_Position = uProjection * uView * uModel * p;
	})";

	static const char* fragmentShaderSource = R"(#version 410 core
	in float Height;
	
	out vec4 FragColor;

	void main()
	{
		float hei = (Height + 16) / 64.0f;
		FragColor = vec4(hei, hei, hei, 1.0f);
	})";
};
