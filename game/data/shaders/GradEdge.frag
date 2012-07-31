#define KERNEL_SIZE 9

uniform sampler2D imgColor;
uniform sampler2D gradient;

uniform float threshold_d;
uniform float threshold_n;
uniform vec4 edgeColor;

void main()
{
	vec2 depNorm = texture2D(gradient, gl_TexCoord[0].st).rg;
	float td = depNorm.r/threshold_d;
	float tn = depNorm.g/threshold_n;

	float edge_d = min(td*td, 1.0);
	float edge_n = min(tn, 1.0);

	if (edge_n == 1.0 || edge_d == 1.0)
		gl_FragColor = edgeColor;
	else
		gl_FragColor = texture2D(imgColor, gl_TexCoord[0].st);
}
