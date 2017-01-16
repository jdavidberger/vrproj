#version 430
uniform mat4 matrix;
uniform float tx4to3[5 * 5];
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 v2UVcoordsIn;
layout(location = 2) in vec4 v3NormalIn;
out vec3 v2UVcoords;
out vec4 normal;
out vec4 pos;

vec4 transform(vec4 inputVec) {
	float pos4[5];
	
	for(int j = 0;j < 5;j++) {
		pos4[j] = 0;
		for(int i = 0;i < 4;i++) {
			pos4[j] = pos4[j] + tx4to3[j * 5 + i] * inputVec[i]; 
		}
		pos4[j] = pos4[j] + tx4to3[j * 5 + 4]; 
	}
	return vec4(pos4[0] / pos4[4],
								pos4[1] / pos4[4],
								pos4[2] / pos4[4],
								pos4[3] / pos4[4]);
}

void main()
{	
	v2UVcoords = v2UVcoordsIn;
	
	gl_Position = matrix * transform(position);
	pos =	transform(position);
	normal = transform(position + v3NormalIn) - pos; 
}
