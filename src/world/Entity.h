/*
	Entity
	The base class that desribes any sort of entity within the
	game world that exists in time and space.

	Created By: Greg Hoffman
*/

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <vector>
#include "../util/vec3.h"
#include "../physics/Force.h"
#include "../util/AABoundingBox.h"
#include "../util/Ray.h"
#include "../util/curve.h"
using namespace::std;

#define ENTITY_TYPE_ORB					0
#define ENTITY_TYPE_MOVING_PLATFORM		1
#define ENTITY_TYPE_JUMP_PAD    		2
#define ENTITY_TYPE_PLAYER				3
#define ENTITY_TYPE_TERRAIN				4
#define ENTITY_TYPE_OTHER				5

class Entity
{
public:
    Entity();
    ~Entity();


    //--------------------------------------------------------------------------
    // Accessors and Mutators ready for auto-inline goodness!
    Vec3f getPos() const
    {
        return position;
    }
    void setPos(const Vec3f& pos);
    float getDir()
    {
        return direction;
    }
    Vec3f getDirVec3f();
    void setDir(float dir)
    {
        direction = dir;
    }
    Vec3f getVel() const
    {
        return velocity;
    }
    void setVel(const Vec3f& vel)
    {
		velocity = vel;
    }
    Vec3f getAcc() const
    {
        return acceleration;
    }
    void setAcc(const Vec3f& acc)
    {
        acceleration = acc;
    }
    std::vector<Force*> getForces() const
    {
        return forces;
    }
    void setForces(const vector<Force*>& frcs)
    {
        forces = frcs;
    }
    void removeForce(Force* force);
    float getMass() const
    {
        return mass;
    }
    void setMass(float m)
    {
        mass = m;
    }
    time_t getLastUpdateTime() const
    {
        return lastUpdateTime;
    }
    void setLastUpdateTime(time_t t)
    {
        lastUpdateTime = t;
    }
    int getType() const
    {
        return type;
    }
    void setType(int t)
    {
        type = t;
    }
    void setInactive()
    {
        active = false;
    }
    void setActive()
    {
        active = true;
    }
    bool isActive() const
    {
        return active;
    }
    void setOnGround()
    {
        onGround = true;
    }
    void setOffGround()
    {
        onGround = false;
    }
    bool isOnGround() const
    {
        return onGround;
    }

    //--------------------------------------------------------------------------
    // Axis Aligned Bounding Box
    AABoundingBox getAABB() const;
    void setAABB(const AABoundingBox& a);

    //--------------------------------------------------------------------------
    // Hit Detection
    virtual void hitEntity(Entity* entity) {}
    virtual void hitTerrain(const Vec3f& dir, const Vec3f& hitPoint, const Triangle& tri) {}

    void setPath(Curve<Vec3f> *p) {pathVar = p;}

protected:
    Vec3f position;
    Vec3f velocity;
    Vec3f acceleration;
    vector<Force*> forces;
	   Curve<Vec3f> *pathVar;
    float direction; //in degrees about the y-axis
    float mass;
    time_t lastUpdateTime;
    AABoundingBox aabb;
	int type;
    bool active;
    bool onGround;
   
};

#endif
