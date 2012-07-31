uniform sampler1D cel;
varying vec3 normal;
varying vec3 ecPos;

void mrt(vec4);

void main()
{
   vec3 lightdir = normalize(vec3(gl_LightSource[0].position) - ecPos);
   float intensity = max(dot(normal, lightdir), 0.0);
   
   vec4 color = gl_Color;
   vec4 diffuse = texture1D(cel, intensity);
      
   //gl_FragColor = color * diffuse;
   mrt(color * diffuse);

}
