#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    vec4 pos = projection * vec4(vertex.xy, 0.0, 1.0);
	pos.z = 0.0;
	gl_Position = pos;
    TexCoords = vertex.zw;
}  
