#version 410
uniform mat4 matrix;
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 v2UVcoordsIn;
layout(location = 2) in vec3 v3NormalIn;
out vec3 v2UVcoords;
out vec4 normal;
out vec4 pos;
void main()
{
	v2UVcoords = v2UVcoordsIn;
	normal = vec4(v3NormalIn, 1.0);
	gl_Position = matrix * position;
	pos = gl_Position;
}
