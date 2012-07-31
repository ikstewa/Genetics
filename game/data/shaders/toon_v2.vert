
varying vec2 texCoord;
//varying vec3 normal;
//varying vec4 glColor;

//varying vec3 ecPos;

void mrt();

void main()
{

   //ecPos = vec3(gl_ModelViewMatrix * gl_Vertex);

   //normal = gl_NormalMatrix * gl_Normal;
   
   //glColor = gl_Color;

   texCoord = gl_MultiTexCoord0.st;

   //gl_Position = ftransform();

   mrt();
}
