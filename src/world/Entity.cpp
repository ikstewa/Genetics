#include <GL/glut.h>
#include "Entity.h"

//------------------------------------------------------------------------------
// Constructor and Destructor
Entity::Entity()
{
    lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
    position = Vec3f();
    direction = 0;
    velocity = Vec3f();
    acceleration = Vec3f();
    mass = 0;
    aabb = AABoundingBox(position,0,0,0);
    active = true;
    onGround = false;

    Force* gravity = new Force(Vec3f(0,-200000,0), INT_MAX);
    forces.push_back( gravity );
}

Entity::~Entity()
{
    for each (Force* f in forces)
    {
        delete f;
    }
}

void Entity::setPos(const Vec3f& pos)
{
    position = pos;
    aabb.setPosition(position);
}

Vec3f Entity::getDirVec3f()
{
    Vec3f dir = Vec3f();
    dir.x = cos(direction*3.14159265/180); //cos() takes radians
    dir.z = sin(direction*3.14159265/180); //sin() takes radians
    return dir;
}

void Entity::setAABB(const AABoundingBox& a)
{
    aabb = a;
    aabb.setPosition(getPos());
}

AABoundingBox Entity::getAABB() const
{
    return aabb;
}

void Entity::removeForce(Force* f)
{
    int i = 0;
    int toRemove;
    for each( Force* force in forces )
    {
        if( f == force )
        {
            toRemove = i;
        }
        ++i;
    }
    forces.erase(forces.begin() + toRemove);
}