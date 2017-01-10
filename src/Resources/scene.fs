#version 410 core
uniform sampler2D mytexture;
in vec2 v2UVcoords;
out vec4 outputColor;
void main()
{
    //outputColor = vec4(1.0, 0.0, 0.4, 1.0);
   outputColor = vec4(v2UVcoords, 0.0, 1.0);
//  outputColor = texture(mytexture, v2UVcoords);
}