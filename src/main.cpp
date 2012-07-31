#include <stdlib.h>
#include <GL/glew.h>
#include "engine/GameEngine.h"
#include "render/ShaderLibrary.h"
#include "render/TextureLibrary.h"

int main()
{
   GameEngine * engine = new GameEngine(  800,600,100,100,
                                          "Genetics",
                                          GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH,
                                          "800x600:32");

   engine->callMainLoop();

}