#include "ChaseCamera.h"
#include "../util/Matrix.h"

#define PI 3.14159265

ChaseCamera::ChaseCamera( Player* target, float offset, float distance ) :
    Camera( 45.f, 0, 0, 0, 0, 0, 0, 1.0, 5000.f ) // TODO: zNear, zFar
{
    player = target;
    yOffset = offset;
    dist = distance;

    up.x = 0;
    up.y = 1;
    up.z = 0;
}

ChaseCamera::~ChaseCamera()
{
}

Vec3f ChaseCamera::getLoc() const
{
    // TODO: angles
    //return player->getAABB().getCenter() + (player->getDir())*(-dist);

    //Vec3f dir = player->getDir() * -1.0;
    
    //dir.y = cos(angle*PI/180.0)*dir.y - sin(angle*PI/180.0)*dir.z;
    //dir.z = sin(angle*PI/180.0)*dir.y + cos(angle*PI/180.0)*dir.z;
    Vec3f dir = player->getDirVec3f() * -1;
    dir += Vec3f(0,yOffset,0);
    dir.normalize();
    
    // quick hack to prevent camera from entering ground
    Vec3f pos = player->getPos() + dir * dist ;
    if (pos.y < 1.0)
        pos.y = 1.0;
    return pos;
}

Vec3f ChaseCamera::getDir() const
{
    // TODO
    return Vec3f();
}

Vec3f ChaseCamera::getLookAt() const
{
	//Vec3f yOffset = Vec3f(0, 4, 0);
   return player->getPos();
}