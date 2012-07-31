#include "Player.h"
#include "ChaseCamera.h"
#include "../GlobalSettings.h"
#include <sstream>

//-----------------------------------------------------------------------------
// Constructor and Destructor
Player::Player()
{
    //chaseCam = new Camera(60,      // fov
    //                    0,7,10,  // position
    //                    0,0,-1,  // direction
    //                    1,100 ); // zNear, zFar

    chaseCam = new ChaseCamera(this, -1.f, 35.f);
	setType(ENTITY_TYPE_PLAYER);
    orbsCollected = 0;
	jumpCount = 0;
    timeToLive = STARTING_TIME_TO_LIVE;
    timeToLiveRate = 1.0;
    mp = (MovingPlatform*)0;
    currentMaxSpeed = STARTING_RUN_SPEED;
    currentMaxJumpForce = STARTING_MAX_JUMP_FORCE;
    drawSkin = true;
    won = false;
    MQ = MessageQueue::getInstance();
}
Player::~Player()
{
    delete chaseCam;
}
//-----------------------------------------------------------------------------
// Hit Detection
void Player::hitEntity(Entity* e)
{
    std::stringstream msg;
    SoundManager* soundManager = SoundManager::getInstance();

    if( e->getType() == ENTITY_TYPE_ORB )
    {
        e->setInactive();
        orbsCollected++;
#if ENABLE_PARTICLES
        ParticleSystemManager* pSysMan = ParticleSystemManager::getInstance();
		Vec3f color;
		if (((Orb*)e)->getOrbType() == ORB_TYPE_ADD_TIME)
			color = Vec3f(0.8, 0.3, 0.0);
		else if (((Orb*)e)->getOrbType() == ORB_TYPE_INCR_JUMP)
			color = Vec3f(0.3, 0.8, 0.0);
        //Vec3f color = Vec3f(0.8, 0.3, 0.0);
        pSysMan->setupOrbCollect(e->getPos(), position, color);
#endif
        //std::cout << "Player is hitting an Orb." << std::endl;
        int orbType = ((Orb*)e)->getOrbType();
        if(orbType == ORB_TYPE_ADD_TIME)
        {
            timeToLive += ((Orb*)e)->getOrbTypeData();
            msg << "Orb collected! " << (int)(((Orb*)e)->getOrbTypeData()) << " seconds added";
            MQ->showMessage(MESSAGE_TIME_ORB,position);
            soundManager->playSound(SOUND_TIME_ORB);
        } else if(orbType == ORB_TYPE_SLOW_TIME)
        {
            float data= ((Orb*)e)->getOrbTypeData();
            timeToLiveRate *= data;
            msg << "Orb collected! Time slowed by " << (int)( data * 100.0 ) << " percent";
        } else if(orbType == ORB_TYPE_INCR_SPD)
        {
            currentMaxSpeed += (int)(((Orb*)e)->getOrbTypeData());
            msg << "Orb collected! Max run speed increased";
            //MQ->addMessage(new Message(msg.str(),3,0));// 0 should be MESSAGE_PIORITY_LOW, is not working
        } else if(orbType == ORB_TYPE_INCR_JUMP)
        {
            currentMaxJumpForce += (int)(((Orb*)e)->getOrbTypeData()); 
            msg << "Orb collected! Max jump height increased";
            MQ->showMessage(MESSAGE_JUMP_ORB,position);
            soundManager->playSound(SOUND_JUMP_ORB);
        } else if(orbType == ORB_TYPE_ABILITY)
        {
            //no special abilities implemented yet
        } else if( orbType == ORB_TYPE_FINAL_ORB )       
        {
            //TODO: GAME OVER YOU WIN!
            won = true;
            MQ->showMessage(MESSAGE_WIN,Vec3f());
            //soundManager->playSound(SOUND_FINAL_ORB);
        }
    } else if( e->getType() == ENTITY_TYPE_JUMP_PAD)
    {
        AddClip(0,0, 20);
        vector<Force*> forces = getForces();
        forces.push_back(new Force( ((JumpPad*)e)->getJumpForce(),((JumpPad*)e)->getJumpDur()));
        setForces(forces);
        soundManager->playSound(SOUND_JUMP_PAD);
    } else if( e->getType() == ENTITY_TYPE_MOVING_PLATFORM)
    {
        setJumpCount(getJumpCount()-1); //Allow for jumping off platforms.
        setOnPlatform(e);
        platformTime = glutGet(GLUT_ELAPSED_TIME);
        //prevent falling through it
        Vec3f v = getVel();

        //move player to be on top of it
        float top = ((MovingPlatform*)e)->getTop();
        if((getAABB().getMinY() < top) && (v.y <= 0.0))  //do not force the player onto platform if his velocity is upwards
        {
            Vec3f pos = getPos();
            pos.y = top + getAABB().getHeight() / 2.0;
            setPos(pos);
        }

        if(v.y < 0.0) 
            v.y = 0.0;
        setVel(v);
    }
}

void Player::moveByPlatform()
{
    if(mp!=0)
    {
        setPos(getPos() + mp->getDeltaPos(platformTime));
        platformTime = glutGet(GLUT_ELAPSED_TIME);
    }
}

void Player::hitTerrain(const Vec3f& dir, const Vec3f& hitPoint, const Triangle& tri)
{
    //TODO: REMOVE TRIANGLE PARAMETER

    SoundManager* soundManager = SoundManager::getInstance();

    // Vertical hit detection; stop downward movement
    if( dir.y < 0.0 )
    {
		//Hack for decrementing jump count, possible bug if you land a jump traveling slower than -20 downwards)
		if( velocity.y < -5.0 )
        {
			setJumpCount(getJumpCount()-1);
        }

        if( velocity.y < -20.0 )
            soundManager->playSound(SOUND_LANDING_CHARACTER);

      if(hitPoint.y + (aabb.getHeight()/2.0) > position.y)
      {
         setPos(Vec3f(position.x, hitPoint.y + (aabb.getHeight()/2.0), position.z)); //Use setPos() to keep AABB position up to date.
      }
      if (ENABLE_PARTICLES && velocity.y < -(getJumpForce()/50000))
      {
         ParticleSystemManager* pSysMan = ParticleSystemManager::getInstance();
         pSysMan->setupDustCloud(Vec3f(position.x, hitPoint.y, position.z), velocity);
         soundManager->playSound(SOUND_LANDING_ENVIRONMENT);
      }
      if( velocity.y < 0.0 ) 
         velocity.y = 0.0;
      if( acceleration.y < 0.0 ) 
         acceleration.y = 0.0;
    }
    // All horizontal hit detection.
    else if( dir.x > 0.0 )
    {
        setPos(Vec3f(hitPoint.x-(aabb.getWidth()/2.0), position.y, position.z));
    }
    else if( dir.x < 0.0 )
    {
        setPos(Vec3f(hitPoint.x+(aabb.getWidth()/2.0), position.y, position.z));
    }
    else if( dir.z > 0.0 )
    {
        setPos(Vec3f(position.x, position.y, hitPoint.z-(aabb.getDepth()/2.0)));
    }
    else if( dir.z < 0.0 )
    {
        setPos(Vec3f(position.x, position.y, hitPoint.z+(aabb.getDepth()/2.0)));
    }
}

float Player::update()
{
   timeToLive -= timeToLiveRate * difftime(time(NULL),lastTime);
   lastTime = time(NULL);
   if(timeToLive < 0)
       timeToLive = 0;
   return timeToLive;
}

void Player::setJumpCount(int count)
{
	if( count >= 0 )
		jumpCount = count;
}
