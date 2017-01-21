#version 410 core
in vec4 pos;
out vec4 outputColor;
void main()
{
	float dist = sqrt(pos.x*pos.x + pos.z*pos.z);
    float rad = sin(3.14 * dist); 	
	float lineSize = .01;
	float antialias = 0.1;
	float d = abs(rad - lineSize / 2.0);
	float t = lineSize / 2.0 - antialias;
	d -= t;
	vec4 color = vec4(0.1, 0.1, 0.1, .7);
	vec4 bgcolor = vec4(1., 1., 1., .7);
	if( d <= 0.0) {	
		outputColor = vec4(color.rgb, 1.0);
	} else {
		d /= antialias;		
		float a = exp(-d*d)*color.a;
		float b = 1.0 - a;
	    outputColor = color * a + bgcolor * b; 
	}        
}