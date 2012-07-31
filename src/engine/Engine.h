//-----------------------------------------------------------------------------
// Engine.h
// 10/3/08
//
// Ian Stewart
// istewart@calpoly.edu
//
//
//-----------------------------------------------------------------------------

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "keyboard.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

// TODO: Change or move point..
struct Point2D
{
public:
   int x, y;
};

class Engine
{

public:

   Engine(  int w, int h, int x, int y, char * title,
            unsigned int mode, const char* gameMode );
   ~Engine();

   void callMainLoop();


   //----------
   // Input
   // Gets the current key status. See keybaord.h
   bool getKeyState(KeyEnum k);
   // return the mouse status of the key: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
   bool getMouseKeyState(GLenum key);
   Point2D getMousePosition(void);

   // helper function to display text
   static void drawText(int x, int y, void* font, char* text);
   static void toggleGameMode();

   virtual void Setup() {}
   virtual void Reshape(int w, int h) {}
protected:

   //-----------------
   // Instance Methods
   //virtual void resize();
   virtual void display() {}
   
   
   virtual void UpdateLoop() {}

   // keep instance for static callback bullshit
   static Engine* instance;

   char * title;
   static int width;
   static int height;

   static const char* gameMode;

   static void init();

   //----------
   // Input
   static bool keyboardButtonStates[];
   static bool mouseStates[];
   static Point2D mousePosition;

   static void hDisplay(void);
   static void hReshape(int w, int h);
   static void hKeyboardDown(unsigned char key, int x, int y);
   static void hSpecialDown(int key, int x, int y);
   static void hKeyboardUp(unsigned char key, int x, int y);
   static void hSpecialUp(int key, int x, int y);
   static void hMouse(int button, int state, int x, int y);
   static void hPassiveMotion(int x, int y);
   static void hTimerCallback(int val);
    
};

#endif
