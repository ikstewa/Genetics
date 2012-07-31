/*
	Player
	The base player class

	Created By: Greg Hoffman
*/

#ifndef _PLAYER_H_
#define _PLAYER_H_

#define STARTING_TIME_TO_LIVE       60.0    //1 minute for debug, probably go with ~10 later
#define MAX_TIME_TO_LIVE            60.0    // it doesn't quite max here, but it's the max displayable time to live.
#define STARTING_RUN_SPEED          24      //the starting speed for keyboard movement
#define STARTING_MAX_JUMP_FORCE     2050000 //the starting jumping force to be applied with 'spacebar'

#include <GL/glew.h>
#include <GL/glut.h>
#include "../util/vec3.h"
#include "AnimatedEntity.h"
#include "Orb.h"
#include "JumpPad.h"
#include "../engine/Camera.h"
#include <time.h>
#include "MovingPlatform.h"
#include "../util/Triangle.h"
#include "../particle/ParticleSystemManager.h"
#include "../util/MessageQueue.h"
#include "../sound/SoundManager.h"

class Player : public AnimatedEntity
{
public:
    Player();
    ~Player();
    void hitEntity(Entity* entity);
    void hitTerrain(const Vec3f& dir, const Vec3f& hitPoint, const Triangle& tri);

    Camera* getCamera() const
    {
        return chaseCam;
	}
	int getJumpCount() const
	{
		return jumpCount;
	}
	void setJumpCount(int count);
    int getOrbCount() const
    {
        return orbsCollected;
    }
    float getTimeToLive() const
    {
        return timeToLive;
    }
    float getMaxTimeToLive() const
    {
        return MAX_TIME_TO_LIVE;
    }
    float update();
    void applyTimeToLiveRate(float r) {timeToLiveRate *= r;}
    void setOnPlatform(Entity *e) {mp = (MovingPlatform*)e;}
    bool isOnPlatform() const {return mp != 0;}
    void moveByPlatform();
    void setMessageQueue(MessageQueue *m) {MQ = m;}
    int getSpeed() {return currentMaxSpeed;}
    int getJumpForce() {return currentMaxJumpForce;}
    void startClock() {lastTime = time(NULL);}
    bool hasWon() const {return won;};

    bool drawSkin;

private:
	int jumpCount;
    int orbsCollected;
    Camera* chaseCam;
    float timeToLive; //in seconds
    float timeToLiveRate;  //multiple of the seconds passing by that will be deducted from timeToLive
    time_t lastTime;
    MovingPlatform *mp;
    int platformTime;
    MessageQueue *MQ;
    int currentMaxJumpForce;
    int currentMaxSpeed;
    bool won;
};

#endif