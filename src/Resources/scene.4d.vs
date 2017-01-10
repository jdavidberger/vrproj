#version 430
uniform mat4 matrix;
uniform float tx4to3[5 * 5];
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 v2UVcoordsIn;
layout(location = 2) in vec3 v3NormalIn;
out vec2 v2UVcoords;

void main()
{
	v2UVcoords = v2UVcoordsIn;
	float pos4[5];
	
	for(int i = 0;i < 5;i++) {
		pos4[i] = 0;
		for(int j = 0;j < 4;j++) {
			pos4[i] = pos4[i] + tx4to3[j * 5 + i] * position[j]; 
		}
		pos4[i] = pos4[i] + tx4to3[4 * 5 + i]; 
	}
	/*
	pos4[0] = position.x; 
	pos4[1] = position.y; 
	pos4[2] = position.z; 
	pos4[3] = position.w; 
	pos4[4] = 1.0;
	*/
	gl_Position = matrix * vec4(pos4[0] / pos4[4],
								pos4[1] / pos4[4],
								pos4[2] / pos4[4],
								pos4[3] / pos4[4]);
}
