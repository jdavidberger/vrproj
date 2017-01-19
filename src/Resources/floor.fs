#version 410 core
uniform sampler2D mytexture;
in vec3 v2UVcoords;
in vec4 normal;
in vec4 pos;
out vec4 outputColor;
void main()
{
    float rad = cos( 3.14 * (pos.x*pos.x + pos.y*pos.y + pos.z*pos.z)); 
    if(rad > -0.1 && rad < 0.1)
        outputColor = vec4(0.9, 0.9, 0.9, .9);
    else 
        outputColor = vec4(0.0, 0.0, 0.0, 0.0);
}