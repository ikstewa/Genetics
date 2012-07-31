// A simple vertex shader
#version 110

varying vec3 normal;
varying float eyeZ;

varying vec3 ecPos;

void mrt()
{
	ecPos = vec3(gl_ModelViewMatrix * gl_Vertex);

	// don't you just love magic numbers??
	//eyeZ = (gl_ModelViewMatrix * gl_Vertex).z / -100.0;
	eyeZ = ecPos.z / -250.0;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[2] = gl_TextureMatrix[2]*gl_Vertex;

	normal = gl_NormalMatrix * gl_Normal;
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}

// Added to allow mrt.vert to be used as a main program
void main()
{
	mrt();
}
