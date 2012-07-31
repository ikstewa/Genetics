//#define MULTI_SAMPLE
#define SAMPLES 25

uniform sampler2D tex;
uniform sampler1D cel;
uniform sampler2D shadowMap;
varying vec3 normal;
varying vec3 ecPos;

uniform float enableShadows;

vec4 shadowTexCoord;

void mrt(vec4);

float lookup(float x, float y)
{
   float v = texture2D(shadowMap, shadowTexCoord.xy + vec2(x, y)).x;
   return (v > shadowTexCoord.z ? 1.0 : 0.4);
}


void main()
{

#ifdef MULTI_SAMPLE
   float blur_spread[5];
   blur_spread[0] = -1.0/2048;
   blur_spread[1] = -0.75/2048;
   blur_spread[2] =  0.0;
   blur_spread[3] =  0.75/2048;
   blur_spread[4] = -1.0/2048;
#endif


   // Shadow map coords
   shadowTexCoord = (gl_TexCoord[2]/gl_TexCoord[2].w)*enableShadows;
   //shadowTexCoord = (shadowTexCoord+1.0) * 0.5;
#ifndef MULTI_SAMPLE
   vec4 shadow = texture2D(shadowMap, shadowTexCoord.xy);
#endif
   shadowTexCoord -= 0.001;


   // calc toon shading
   vec3 lightdir = normalize(vec3(gl_LightSource[0].position) - ecPos);
   float a = max(dot(normal, lightdir), 0.0);
   vec4 diffuse = texture1D(cel, a);
   
   // material color
   vec4 color = texture2D(tex, gl_TexCoord[0].st) * gl_Color;


   #ifdef MULTI_SAMPLE

   float samples = 1.0/(SAMPLES);
   
   float shade = 0.0;

   shade += samples*lookup(blur_spread[0], blur_spread[0]);
   shade += samples*lookup(blur_spread[0], blur_spread[1]);
   shade += samples*lookup(blur_spread[0], blur_spread[2]);
   shade += samples*lookup(blur_spread[0], blur_spread[3]);
   shade += samples*lookup(blur_spread[0], blur_spread[4]);
   shade += samples*lookup(blur_spread[1], blur_spread[0]);
   shade += samples*lookup(blur_spread[1], blur_spread[1]);
   shade += samples*lookup(blur_spread[1], blur_spread[2]);
   shade += samples*lookup(blur_spread[1], blur_spread[3]);
   shade += samples*lookup(blur_spread[1], blur_spread[4]);
   shade += samples*lookup(blur_spread[2], blur_spread[0]);
   shade += samples*lookup(blur_spread[2], blur_spread[1]);
   shade += samples*lookup(blur_spread[2], blur_spread[2]);
   shade += samples*lookup(blur_spread[2], blur_spread[3]);
   shade += samples*lookup(blur_spread[2], blur_spread[4]);
   shade += samples*lookup(blur_spread[3], blur_spread[0]);
   shade += samples*lookup(blur_spread[3], blur_spread[1]);
   shade += samples*lookup(blur_spread[3], blur_spread[2]);
   shade += samples*lookup(blur_spread[3], blur_spread[3]);
   shade += samples*lookup(blur_spread[3], blur_spread[4]);
   shade += samples*lookup(blur_spread[4], blur_spread[0]);
   shade += samples*lookup(blur_spread[4], blur_spread[1]);
   shade += samples*lookup(blur_spread[4], blur_spread[2]);
   shade += samples*lookup(blur_spread[4], blur_spread[3]);
   shade += samples*lookup(blur_spread[4], blur_spread[4]);
   
   shade *= diffuse.x;

   #else
   // Shadow effect
   float shade = 0;
   if ((shadow.x) < shadowTexCoord.z)
      shade = 0.4;
   else
      shade = diffuse.x;
   #endif


   mrt(color * shade);
}
