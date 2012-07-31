#define KERNEL_SIZE 9

uniform sampler2D imgColor;
uniform sampler2D gradient;

uniform float width;
uniform float height;
uniform float threshold_d;
uniform float threshold_n;
uniform vec4 edgeColor;

uniform bool enableEdge;

float step_w = 1.0/width;
float step_h = 1.0/height;
vec2 offset[KERNEL_SIZE];

float gdMax, gdMin;
float gnMax, gnMin;

void findMaxMin()
{
	gdMax = texture2D(gradient, gl_TexCoord[0].st + offset[0]).r;
	gdMin = texture2D(gradient, gl_TexCoord[0].st + offset[0]).r;
	gnMax = texture2D(gradient, gl_TexCoord[0].st + offset[0]).g;
	gnMin = texture2D(gradient, gl_TexCoord[0].st + offset[0]).g;

 	float dep;
	float norm;
	
	for (int i = 1; i < KERNEL_SIZE; i++)
	{
		vec4 t = texture2D(gradient, gl_TexCoord[0].st + offset[i]);
		dep = t.r;
		norm = t.g;

		if (dep > gdMax)
			gdMax = dep;
		if (dep < gdMin)
			gdMin = dep;

		if (norm > gnMax)
			gnMax = norm;
		if (norm < gnMin)
			gnMin = norm;
	}
}

void main()
{
if (enableEdge)
{
	// set the offsets
	offset[0] = vec2(-step_w, -step_h);
	offset[1] = vec2(0.0, -step_h);
	offset[2] = vec2(step_w, -step_h);
	offset[3] = vec2(-step_w, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(step_w, 0.0);
	offset[6] = vec2(-step_w, step_h);
	offset[7] = vec2(0.0, step_h);
	offset[8] = vec2(step_w, step_h);

/* // Try a faster edge
	findMaxMin();

	float td = (gdMax-gdMin)/threshold_d;
	float tn = (gnMax-gnMin)/threshold_n;
*/

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
else
{
	gl_FragColor = texture2D(imgColor, gl_TexCoord[0].st);
}
}
