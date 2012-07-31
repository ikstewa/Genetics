/*
	Moving Platform
	The base moving platform class

	Created By: Daniel White
*/

#ifndef _MOVING_PLATFORM_H_
#define _MOVING_PLATFORM_H_

#include "../util/curve.h"
#include "RenderedEntity.h"
using namespace::std;

class MovingPlatform : public RenderedEntity
{
public:
    MovingPlatform(Vec3f start, Vec3f inPoint, Vec3f outPoint, Vec3f end, float w, float l, float h, float s);
    ~MovingPlatform();
    void hitEntity(Entity* entity);
    Vec3f getPos() const; //overrides getPos in Entity because the platform moves
    Vec3f getDeltaPos(int lastTime) const; //for when player is on platform
    float getRadius() const; //special radius for moving platform
    //Vec3f getDimensions() const; 
    Vec3f getPathCenter() const;
    float getTop() {return getPos().y + getAABB().getHeight()/2.0f;}

    //--------------------------------------------------------------------------
    // Axis Aligned Bounding Box
    AABoundingBox getAABB();

private:
	Curve<Vec3f> path;
	float radius;
	float width;
	float length;
	float height;
	float speed;
	int creationTime;

    Vec3f pathCenter;
};

#endif