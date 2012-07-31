#define KERNEL_SIZE 9

uniform sampler2D depColor;
uniform sampler2D normColor;

uniform float width;
uniform float height;

float step_w = 1.0/width;
float step_h = 1.0/height;


vec2 offset[KERNEL_SIZE];

float depth[KERNEL_SIZE];
vec3 norm[KERNEL_SIZE];

float d, n;


float compareNorm(vec3 n1, vec3 n2)
{
//	float a = dot(n1, n2)/(-2.0) + 1.0;
//	if (a > 1.0 || a < 0.0 )
//		outColor = vec4(1.0, 1.0, 1.0, 1.0);
	return dot(n1, n2)/(-2.0) + 1.0;
}

void main()
{
	offset[0] = vec2(-step_w, -step_h);
	offset[1] = vec2(0.0, -step_h);
	offset[2] = vec2(step_w, -step_h);
	offset[3] = vec2(-step_w, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(step_w, 0.0);
	offset[6] = vec2(-step_w, step_h);
	offset[7] = vec2(0.0, step_h);
	offset[8] = vec2(step_w, step_h);


	// Load the surrounding pixels
	depth[0] = texture2D(depColor, gl_TexCoord[0].st + offset[0]).r;
	norm[0] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[0]).rgb);
	depth[1] = texture2D(depColor, gl_TexCoord[0].st + offset[1]).r;
	norm[1] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[1]).rgb);
	depth[2] = texture2D(depColor, gl_TexCoord[0].st + offset[2]).r;
	norm[2] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[2]).rgb);
	depth[3] = texture2D(depColor, gl_TexCoord[0].st + offset[3]).r;
	norm[3] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[3]).rgb);
	depth[4] = texture2D(depColor, gl_TexCoord[0].st + offset[4]).r;
	norm[4] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[4]).rgb);
	depth[5] = texture2D(depColor, gl_TexCoord[0].st + offset[5]).r;
	norm[5] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[5]).rgb);
	depth[6] = texture2D(depColor, gl_TexCoord[0].st + offset[6]).r;
	norm[6] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[6]).rgb);
	depth[7] = texture2D(depColor, gl_TexCoord[0].st + offset[7]).r;
	norm[7] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[7]).rgb);
	depth[8] = texture2D(depColor, gl_TexCoord[0].st + offset[8]).r;
	norm[8] = normalize(texture2D(normColor, gl_TexCoord[0].st + offset[8]).rgb);

	d = (1.0/8.0)*(abs(depth[0]-depth[4])
		+ 2.0* abs(depth[1]-depth[4])
		+      abs(depth[2]-depth[4])
		+ 2.0* abs(depth[3]-depth[4])
		+ 2.0* abs(depth[5]-depth[4])
		+      abs(depth[6]-depth[4])
		+ 2.0* abs(depth[7]-depth[4])
		+      abs(depth[8]-depth[4]));


	n = (1.0/16.0)*(compareNorm(norm[0],norm[4])
		+ 2.0* compareNorm(norm[1],norm[4])
		+      compareNorm(norm[2],norm[4])
		+ 2.0* compareNorm(norm[3],norm[4])
		+ 2.0* compareNorm(norm[5],norm[4])
		+      compareNorm(norm[6],norm[4])
		+ 2.0* compareNorm(norm[7],norm[4])
		+      compareNorm(norm[8],norm[4]));

	
//	gl_FragColor = vec4(0.0, n, 0.0, 1.0);//texture2D(normColor, gl_TexCoord[0].st);

//	gl_FragData[0] = vec4(1.0,1.0,1.0,1.0);
	gl_FragColor = vec4(d, n, 0.0, 1.0);
}
