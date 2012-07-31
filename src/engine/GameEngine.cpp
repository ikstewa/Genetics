//-----------------------------------------------------------------------------
// GameEngine.cpp
// 10/7/08
//
// Ian Stewart
// istewart@calpoly.edu
//
// TODO: everything
//
//-----------------------------------------------------------------------------

#include "GameEngine.h"
#include "../render/ShaderLibrary.h"
#include "../render/TextureLibrary.h"
#include "../GlobalSettings.h"

GameEngine::GameEngine( int w, int h, int x, int y, char * title,
                        unsigned int mode, const char* gameMode ) :
         Engine(w,h,x,y,title,mode,gameMode)
{

    inCutScene = false;
    gameOver=false;
    playWinSound=true;
    renMan = new RenderManager();
    MQ = MessageQueue::getInstance();
    world = new World(renMan);
    world->setMessageQueue(MQ);

    // load texture and shader libraries
    ShaderLibrary::getInstance();
    TextureLibrary::getInstance();

    world->LoadScene(".//data//maps//Level.map");
    renMan->buildBSPTree();

    // Sound loading NOTE: MUST BE IN THIS ORDER.
    SoundManager* sm = SoundManager::getInstance();
    sm->loadMusic(".//data//sound//intro.mid", true, 128);
    sm->loadMusic(".//data//sound//Batman.mid", true, 128);
    sm->loadSound(".//data//sound//time_orb.wav",false,128);
    sm->loadSound(".//data//sound//jump_orb.wav",false,128);
    sm->loadSound(".//data//sound//cool.wav",false,128);
    sm->loadSound(".//data//sound//death.wav",false,128);
    sm->loadSound(".//data//sound//awww.wav",false,128);
    sm->loadSound(".//data//sound//cheering.wav",false,128);
    sm->loadSound(".//data//sound//landing.wav",false,128);
    sm->loadSound(".//data//sound//oof.wav",false,128);
    sm->loadSound(".//data//sound//jump_pad.wav",false,128);

    // GUI
    gui = new Gui(this);
    inCutScene = true;

    Reshape(w, h);
#if ENABLE_SHADOWS
    renMan->setupShadows();
#endif
}

void GameEngine::Setup()
{
    renMan->Setup();
}

void GameEngine::Reshape(int w, int h)
{
  if (!inCutScene)
    renMan->setupView(0, 0, width, height, world->getCamera());
  else
    gui->setupView(width, height);
}

void GameEngine::display()
{
  if (!inCutScene)
  {
    #if ENABLE_TEST_RENDER
    renMan->testRender(0,0,width,height,world->getCamera());
    #else
    renMan->render(0, 0, width, height, world->getCamera());
    #endif
  }
  else
    gui->render(width, height);
}

void GameEngine::UpdateLoop()
{
    Player* player = world->GetPlayer();

    if (keyboardButtonStates[KEY_O])
    {
      RenderableModel::showBone++;
    }

    if (keyboardButtonStates[KEY_P])
    {
      RenderableModel::showBone--;
    }
#if DEVELOPER_KEYS
    //----------------
    // FBO keyboard shortcuts
    if (keyboardButtonStates[KEY_Z])
    {
        renMan->displayTexture = 0;
        keyboardButtonStates[KEY_Z] = false;
    }
    if (keyboardButtonStates[KEY_X])
    {
        renMan->displayTexture = 1;
        keyboardButtonStates[KEY_X] = false;
    }
    if (keyboardButtonStates[KEY_C])
    {
        renMan->displayTexture = 2;
        keyboardButtonStates[KEY_C] = false;
    }
    //-------------------
    // Edge threshold keys
    if (keyboardButtonStates[KEY_N])
    {
        renMan->threshold_d -= 0.001;
        printf("threshold_d: %f\n", renMan->threshold_d);
        keyboardButtonStates[KEY_N] = false;
    }
    if (keyboardButtonStates[KEY_J])
    {
        renMan->threshold_d += 0.001;
        printf("threshold_d: %f\n", renMan->threshold_d);
        keyboardButtonStates[KEY_J] = false;
    }
    if (keyboardButtonStates[KEY_M])
    {
        renMan->threshold_n -= 0.0001;
        printf("threshold_n: %f\n", renMan->threshold_n);
        keyboardButtonStates[KEY_M] = false;
    }
    if (keyboardButtonStates[KEY_K])
    {
        renMan->threshold_n += 0.0001;
        printf("threshold_n: %f\n", renMan->threshold_n);
        keyboardButtonStates[KEY_K] = false;
    }
#endif

    
    if(!player->hasWon() && !gameOver && !inCutScene)
    {
        normalKeyboardInputs(player);
        // update player animation
        player->IncrementAnimation(30.0/1000.0*10);
        // check to play idle animation
        if (!(world->GetPlayer()->GetClips().size() > 0))
        {
          world->GetPlayer()->AddClip(AnimatedEntity::ANIM_IDLE,0, 10);
        } 

        

        //modify players position by platform movement
        if(player->isOnPlatform())
        {
            player->moveByPlatform();
        } 
        //force the player to not be on platform and have to recalculate if the player is indeed on the platform
        player->setOnPlatform((Entity*)0); //not on platform

        // then update physics
        world->updateWorld();

        //update player's time to live
        if(!inCutScene && (gameOver || player->update()<=0.0))
        {
            //printf("Game Over.\n");
            gameOver=true;
            MQ->showMessage(MESSAGE_LOSE, Vec3f());
            renMan->activateLossScreen();
            // 2 should be MESSAGE_PRIORITY_HIGH, #define is broken for me though
            player->setVel(Vec3f(0,0,0));
            SoundManager::getInstance()->playSound(SOUND_LOSE);
            //exit(0);
        }
    }
    else if(inCutScene)
    {
        introKeyboardInputs();
    }
    else if( player->hasWon() )
    {
        player->setVel(Vec3f(0,0,0));
        if( playWinSound )
        {
            SoundManager::getInstance()->playSound(SOUND_WIN);
            playWinSound = false;
        }
    }

    // update mouse position
    mouseX = mousePosition.x;
    mouseY = mousePosition.y;
    
}

void GameEngine::normalKeyboardInputs(Player* player)
{
   vector<Force*> forces = player->getForces();
   Vec3f v = player->getVel();
	bool updateVel = false;
	Vec3f dv3d = player->getDirVec3f();
   int spd = player->getSpeed();
   int jumpForce = player->getJumpForce();
   //-----------------
   // Player movement
	if( player->isOnGround() || player->isOnPlatform() )
	{
      float stored_y = v.y;
      v.y = 0;
		v.x = 0;
		v.z = 0;
		if (keyboardButtonStates[KEY_W] )
		{
			//Move forward
			v += (dv3d*spd);
			world->GetPlayer()->AddClip(AnimatedEntity::ANIM_WALKING,0, 5);
			updateVel = true;
		}
		if (keyboardButtonStates[KEY_A] )
		{
			//Strafe left
			float ori = player->getDir();
		   ori -= 90;
			Vec3f strafe = Vec3f();
			strafe.x = cos(ori*3.14159265/180); //cos() takes radians
			strafe.z = sin(ori*3.14159265/180); //sin() takes radians
			v += (strafe*spd);
			updateVel = true;
		}
		if (keyboardButtonStates[KEY_S] )
		{
         //Move backwards
			v -= (dv3d*spd);
			updateVel = true;
		}
		if (keyboardButtonStates[KEY_D] )
		{
			//Strafe right
			float ori = player->getDir();
			ori += 90;
			Vec3f strafe = Vec3f();
			strafe.x = cos(ori*3.14159265/180); //cos() takes radians
			strafe.z = sinf(ori*3.14159265/180); //sin() takes radians
		   v += (strafe*spd);
			updateVel = true;
		}
      v.normalize();
      v *= spd;
      v.y = stored_y;
      player->setVel(v);
	}
	else //player is in the air, retain momentum
	{
      if (keyboardButtonStates[KEY_W] )
      {
         //Forward
         v.x = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.x + PERCENT_MOVEMENT_APPLIED_IN_AIR*dv3d.x*spd;
         v.z = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.z + PERCENT_MOVEMENT_APPLIED_IN_AIR*dv3d.z*spd;
      }
      if (keyboardButtonStates[KEY_A] )
      {
         //Strafe left
         float ori = player->getDir();
         ori -= 90;
         Vec3f strafe = Vec3f();
         strafe.x = cos(ori*3.14159265/180); //cos() takes radians
         strafe.z = sin(ori*3.14159265/180); //sin() takes radians
         v.x = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.x + PERCENT_MOVEMENT_APPLIED_IN_AIR*strafe.x*spd;
         v.z = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.z + PERCENT_MOVEMENT_APPLIED_IN_AIR*strafe.z*spd;
      }
      if (keyboardButtonStates[KEY_S] )
      {
         //Backwards
         v.x = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.x - PERCENT_MOVEMENT_APPLIED_IN_AIR*dv3d.x*spd;
         v.z = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.z - PERCENT_MOVEMENT_APPLIED_IN_AIR*dv3d.z*spd;
      }
      if (keyboardButtonStates[KEY_D] )
      {
         //Strafe right
         float ori = player->getDir();
         ori += 90;
         Vec3f strafe = Vec3f();
         strafe.x = cos(ori*3.14159265/180); //cos() takes radians
         strafe.z = sinf(ori*3.14159265/180); //sin() takes radians
         v.x = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.x + PERCENT_MOVEMENT_APPLIED_IN_AIR*strafe.x*spd;
         v.z = (1.0-PERCENT_MOVEMENT_APPLIED_IN_AIR)*v.z + PERCENT_MOVEMENT_APPLIED_IN_AIR*strafe.z*spd;
      }
      player->setVel(v);
	}

	if (keyboardButtonStates[KEY_SPACE])
    {
		keyboardButtonStates[KEY_SPACE] = false;
		if( player->getJumpCount() < NUM_JUMPS )
		{
			player->setJumpCount(player->getJumpCount()+1);
			forces.push_back(new Force(Vec3f(0,(float)jumpForce,0),100));
			player->setForces(forces);
			world->GetPlayer()->AddClip(0,0, 20);
		}
	}

    if (mouseStates[GLUT_RIGHT_BUTTON])
    {
        // rotate chase cam
        ChaseCamera *cam = (ChaseCamera*)(player->getCamera());
        float offset = cam->getOffset()+(mousePosition.y - mouseY)*.01f;
        cam->setOffset(offset);

        // rotate character
        float ori = player->getDir();
        ori += (mousePosition.x - mouseX) * .5; //TODO: .5 should be mouse sensitivity variable.
        player->setDir(ori);
        // TODO: set distance??
        //float delta = ;
        //cam->setDist(cam->getDist()+delta);

    }
}
void GameEngine::introKeyboardInputs()
{
  gui->handleInputs();
}
