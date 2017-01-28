#version 410 core
in vec4 pos;
out vec4 outputColor;
void main()
{
	float distXW = sqrt(pos.x*pos.x + pos.w*pos.w);
	float distXZ = sqrt(pos.x*pos.x + pos.z*pos.z);
	float distWZ = sqrt(pos.z*pos.z + pos.w*pos.w);
	float dist = distWZ; 

    float rad = sin(3.14 * dist); 	
	float lineSize = .1;
	float antialias = 0.1;
	float d = abs(rad - lineSize / 2.0);
	float t = lineSize / 2.0 - antialias;
	d -= t;

	vec4 color = vec4( (pos.w + 50.0) / 100.0, 1.0, 1.0, 0.5);
	vec4 bgcolor = vec4(0.0, 0.0, 0.0, 0.1);
	if( d <= 0.0) {	
		outputColor = vec4(color.rgb, 1.0);
	} else {
		d /= antialias;		
		float a = exp(-d*d)*color.a;
		float b = 1.0 - a;
	    outputColor = color * a + bgcolor * b; 
	}      
	outputColor = vec4(0.8, 0.8, 0.8, 1.0);
}