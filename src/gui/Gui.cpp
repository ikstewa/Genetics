#include "../engine/GameEngine.h"
#include "Gui.h"
#include "../render/RenderManager.h"
#include "../render/ShaderLibrary.h"
#include "../render/TextureLibrary.h"
#include "../sound/SoundManager.h"
#include <GL/glew.h>
#include <GL/glut.h>

Gui::Gui(GameEngine *engine)
{
  theEngine = engine;
  cutscenePlaying = false;
  mainMenu = true;
  controls = false;
  leftButton = false;
  SoundManager::getInstance()->playMusic(MUSIC_INTRO);

}

void Gui::setupView(int width, int height)
{
  this->width = width;
  this->height = height;
  setup();
}

bool Gui::isOver(float mx, float maxx, float my, float maxy)
{
  mx = width * (mx / 100.0);
  maxx = width * (maxx / 100.0);
  my = height * (my / 100.0);
  maxy = height * (maxy / 100.0);
  Point2D mouse = theEngine->getMousePosition();
  mouse.y = height - mouse.y; // need to reverse this because of glut

  return mx <= mouse.x && maxx >= mouse.x &&
         my <= mouse.y && maxy >= mouse.y;
}

void Gui::render(int width, int height)
{
  this->width = width;
  this->height = height;
  setup();

  if (mainMenu)
    renderMainMenu();

  if (controls)
    renderControls();

  if (cutscenePlaying)
    renderCutscene();

  glutSwapBuffers();
}

void Gui::renderMainMenu()
{
  ShaderLibrary::getInstance()->attachShader(SHADER_FIXED);
  TextureLibrary::getInstance()->Activate(TEXTURE_DISABLE);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // setup using a Camera object
  gluOrtho2D(0, 100, 0, 100);
  

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1, 1, 1);
  TextureLibrary::getInstance()->Activate(TEXTURE_MAIN_MENU_BACK);

  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(0, 0);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(100, 0);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(100, 100);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(0, 100);
  glEnd();

  TextureLibrary::getInstance()->Activate(TEXTURE_DISABLE);

  // RENDER START GAME
  if (isOver(35, 65, 40, 50))
    glColor3f(.1, .5, .8);
  else
    glColor3f(.05, .3, .6);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(35, 40);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(65, 40);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(65, 50);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(35, 50);
  glEnd();

  // RENDER CONTROLS
  if (isOver(35, 65, 25, 35))
    glColor3f(.1, .5, .8);
  else
    glColor3f(.05, .3, .6);

  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(35, 25);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(65, 25);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(65, 35);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(35, 35);
  glEnd();

  if (isOver(35, 64, 10, 20))
    glColor3f(.1, .5, .8);
  else
    glColor3f(.05, .3, .6);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(35, 10);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(65, 10);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(65, 20);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(35, 20);
  glEnd();

  glColor3f(0, 0, 0);
  RenderManager::drawText(46, 44, GLUT_BITMAP_HELVETICA_12, "Start Game");
  RenderManager::drawText(47, 29, GLUT_BITMAP_HELVETICA_12, "Controls");
  RenderManager::drawText(48, 14, GLUT_BITMAP_HELVETICA_12, "Exit");
}

void Gui::renderControls()
{
  ShaderLibrary::getInstance()->attachShader(SHADER_FIXED);
  TextureLibrary::getInstance()->Activate(TEXTURE_DISABLE);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // setup using a Camera object
  gluOrtho2D(0, 100, 0, 100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1, 1, 1);
  TextureLibrary::getInstance()->Activate(TEXTURE_MAIN_MENU_BACK);

  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(0, 0);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(100, 0);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(100, 100);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(0, 100);
  glEnd();

  TextureLibrary::getInstance()->Activate(TEXTURE_DISABLE);

  // RENDER START GAME
  
  if (isOver(35, 64, 10, 20))
    glColor3f(.1, .5, .8);
  else
    glColor3f(.05, .3, .6);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(35, 10);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(65, 10);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(65, 20);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(35, 20);
  glEnd();

  glColor3f(0, 0, 0);
  RenderManager::drawText(48, 14, GLUT_BITMAP_HELVETICA_12, "Back");

  RenderManager::drawText(45, 60, GLUT_BITMAP_HELVETICA_18, "W - Forward");
  RenderManager::drawText(45, 55, GLUT_BITMAP_HELVETICA_18, "S - Backwards");
  RenderManager::drawText(45, 50, GLUT_BITMAP_HELVETICA_18, "A - Side Step Left");
  RenderManager::drawText(45, 45, GLUT_BITMAP_HELVETICA_18, "D - Side Step Right");
  RenderManager::drawText(40, 40, GLUT_BITMAP_HELVETICA_18, "Space - Jump");
}

void Gui::renderCutscene()
{

}

void Gui::setup()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
  // setup the projection matrix
    // setup using a Camera object
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // setup the viewport
    gluOrtho2D(0, 100, 0, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
}

void Gui::handleInputs()
{
  if (mainMenu)
    menuInputs();

  if (controls)
    controlsInputs();

  if (cutscenePlaying)
    cutsceneInputs();
}

void Gui::controlsInputs()
{
  bool lastLeft = leftButton;
  leftButton = theEngine->getMouseKeyState(GLUT_LEFT_BUTTON);
  // BACK BUTTON
  if (isOver(35, 65, 10, 20) && lastLeft && !leftButton)
  {
    // we got clicked on
    mainMenu = true;
    controls = false;
  }
}

void Gui::menuInputs()
{
  bool lastLeft = leftButton;
  leftButton = theEngine->getMouseKeyState(GLUT_LEFT_BUTTON);
  // CONTROLS BUTTON
  if (isOver(35, 65, 25, 35) && lastLeft && !leftButton)
  {
    // we got clicked on
    mainMenu = false;
    controls = true;
  }

  // START GAME
  if (isOver(35, 65, 40, 50) && lastLeft && !leftButton)
  {
    // we got clicked on start the game!
    theEngine->setInCutScene(false);
    theEngine->Setup();
    theEngine->Reshape(width, height);
    SoundManager::getInstance()->stopMusic(MUSIC_INTRO);
    SoundManager::getInstance()->playMusic(MUSIC_GAME);
	theEngine->startClock();
  }
}

void Gui::cutsceneInputs()
{

}
