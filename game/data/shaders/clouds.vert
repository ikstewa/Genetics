varying float LightIntensity;
varying vec3 MCposition;

uniform vec3 LightPos;
uniform float Scale;

void main()
{
	gl_Position		= ftransform();
	vec3 ECposition	= vec3(gl_ModelViewMatrix * gl_Vertex);
	MCposition		= vec3(gl_Vertex) * Scale;
	vec3 tnorm		= -normalize(vec3(gl_NormalMatrix * gl_Normal));
	LightIntensity	= dot(normalize(LightPos - ECposition), tnorm) * 1.5;
	//LightIntensity = 1.5;
}