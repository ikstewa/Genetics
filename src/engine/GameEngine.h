//-----------------------------------------------------------------------------
// GameEngine.h
// 10/7/08
//
// Ian Stewart
// istewart@calpoly.edu
//
// TODO: everything
//
//-----------------------------------------------------------------------------

#ifndef __GAME_ENGINE_H__
#define __GAME_ENGINE_H__

#include <stdlib.h>
#include <math.h>
#include "Engine.h"
#include "../render/RenderManager.h"
#include "../world/ChaseCamera.h"
#include "../world/World.h"
#include "../util/MessageQueue.h"
#include "../gui/Gui.h"
#include "../sound/SoundManager.h"

//#define MAX_CHARGE_JUMP_TIME                2000.0     //this is the minimum time in ms required to get a maximum jump force
#define NUM_JUMPS							1 //set number of jumps allowed
#define PERCENT_MOVEMENT_APPLIED_IN_AIR     0.02
#define STATIONARY_JUMP_FACTOR				2.0

class GameEngine : public Engine
{

public:
   GameEngine( int w, int h, int x, int y, char * title,
               unsigned int mode, const char* gameMode );

   bool isInCutScene() {return inCutScene;}
   void setInCutScene(bool val) {inCutScene = val;}
   virtual void Setup();
   virtual void Reshape(int w, int h);
   void startClock() {world->startClock();}
protected:

   int mouseX, mouseY;

   //---------------
   // Managers
   RenderManager *renMan;
   World *world;

   //---------------
   // protected methods
   virtual void display();
   
   
   virtual void UpdateLoop();

   void normalKeyboardInputs(Player* player);
   void introKeyboardInputs();

private:

    Gui *gui;

    MessageQueue *MQ;
    bool chargingJump;
    int jumpStartTime; //in ms
    bool inCutScene;
    bool gameOver;
    bool playWinSound;


};

#endif