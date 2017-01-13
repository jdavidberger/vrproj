#version 410 core
uniform sampler2D mytexture;
in vec3 v2UVcoords;
in vec4 normal;
in vec4 pos;
out vec4 outputColor;
void main()
{
    vec3 lightColor = vec3(1., 1., 1.);
    vec4 lightPos = vec4(0.0, 3.0, 0.0, 0.0);
    float intensity = 1.;
    if(dot(normal, normal) > .5) {
        vec4 norm = normalize(normal);
        vec4 lightDir = normalize(lightPos - pos); 
        intensity = max(dot(norm, lightDir), 0.);        
    //outputColor = vec4(1.0, 0.0, 0.4, 1.0);
    }
    vec3 diffuse = intensity * lightColor;
    vec3 ambient = vec3(.4, .4, .4);
   outputColor = vec4((ambient + diffuse) * v2UVcoords, .8);   
//  outputColor = texture(mytexture, v2UVcoords);
}