varying float LightIntensity;
varying vec3 MCposition;

uniform sampler3D Noise;
uniform vec3 SkyColor;
uniform vec3 CloudColor;

uniform int TIME_FROM_INIT;

vec3 Offset = vec3(0.0,0.0,0.0);

void main()
{
	float offset = float(TIME_FROM_INIT) *0.00005;
	Offset = vec3(-offset, offset, offset);
	vec4 noisevec	= texture3D(Noise, MCposition+Offset);
	float intensity	= (noisevec[0] + noisevec[1] + noisevec[2] + noisevec[3] + 0.03125) * 1.0;
	
	vec3 color		= mix(SkyColor, CloudColor, intensity) * LightIntensity;
	color = clamp(color, 0.0, 1.0);
	gl_FragData[0] 	= vec4(color, 1.0);
}
