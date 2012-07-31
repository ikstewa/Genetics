#ifndef _CHASE_CAMERA_H_
#define _CHASE_CAMERA_H_

#include "../engine/Camera.h"
#include "Player.h"

// TODO: Angle is currently not implemented!!

//-----------------------------------------------------------------------------
// Chase Cam to follow a character
class ChaseCamera : public Camera
{

public:

    ChaseCamera( Player* target, float offset, float distance );
    ~ChaseCamera();

    Vec3f getLoc() const;

    Vec3f getDir() const;

    Vec3f getLookAt() const;

    float getDist() const { return dist; }
    void setDist(float d) { dist = d; }
    float getOffset() const { return yOffset; }
    void setOffset(float o) { yOffset = o; }

protected:
    Player* player;
    float yOffset;
    float dist;
};

#endif