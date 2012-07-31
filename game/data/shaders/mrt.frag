#version 110

varying vec3 normal;
varying float eyeZ;

void mrt(vec4 color)
{
	vec3 n;

	n = vec3(normal.x/2.0+1.0, normal.y/2.0+1.0, normal.z/2.0+1.0);
	n = normalize(n);

	float depth = eyeZ;



	gl_FragData[1] = vec4(depth, depth, depth, 1.0);

	gl_FragData[2] = vec4(n, 1.0);

	gl_FragData[0] = color; //gl_Color;
}
