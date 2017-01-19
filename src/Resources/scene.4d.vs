#version 430
uniform mat4 matrix;
uniform float eyeMatrix[5 * 5];
uniform float tx4to3[5 * 5];
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 v2UVcoordsIn;
layout(location = 2) in vec4 v3NormalIn;
out vec3 v2UVcoords;
out vec4 normal;
out vec4 pos;

vec4 down(in float x[5]) {
	return vec4(x[0] / x[4],
				x[1] / x[4],
				x[2] / x[4],
				x[3] / x[4]);
}
void up(out float rtn[5], vec4 x) {
	rtn[0] = x.x;
	rtn[1] = x.y;
	rtn[2] = x.z;
	rtn[3] = x.w;
	rtn[4] = 1.0; 
}
void transform(out float pos5[5], in float tx[5 * 5], in float inputVec[5]) {
	for(int j = 0;j < 5;j++) {
		pos5[j] = 0;
		for(int i = 0;i < 5;i++) {
			pos5[j] = pos5[j] + tx[j * 5 + i] * inputVec[i]; 
		}
	}
}

void main()
{	
	v2UVcoords = v2UVcoordsIn;
	float pose5[5];
	up(pose5, position); 
	
	float scratch[5];
	//transform(pose5, tx4to3, pose5);
	transform(scratch, tx4to3, pose5);
	//transform(pos, eyeMatrix, pose5);
	pos = down(scratch);
	gl_Position = matrix * pos;
	
    float normPose[5];
	up(normPose, 	position + v3NormalIn);
	transform(normPose, tx4to3, normPose); 
	for(int i = 0;i < 5;i++)
		normPose[i] = normPose[i] - scratch[i]; 
	normal = down(normPose);
	
}
