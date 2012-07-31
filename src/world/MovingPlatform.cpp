#include "MovingPlatform.h"
#include <GL/glut.h>

//-----------------------------------------------------------------------------
// Constructor and Destructor
MovingPlatform::MovingPlatform(Vec3f start, Vec3f inPoint, Vec3f outPoint, Vec3f end, float w, float l, float h, float s)
{
    path = Curve<Vec3f>();
    path.AddPoint(start,outPoint,outPoint,0);
    path.AddPoint(end,inPoint,inPoint,100);
    creationTime = glutGet(GLUT_ELAPSED_TIME);
    width = w;
    height = h;
    length = l;
    speed = s;
    radius = (end - start).length() / 2.0 + sqrt(w*w + l*l + h*h);
    setType(ENTITY_TYPE_MOVING_PLATFORM);

    pathCenter = start + (end - start)/2.f;
}
MovingPlatform::~MovingPlatform()
{
}

//-----------------------------------------------------------------------------
// Hit Detection
void MovingPlatform::hitEntity(Entity* entity)
{
   //TODO: entity interaction
}

Vec3f MovingPlatform::getPos() const
{
	int t = speed * ((glutGet(GLUT_ELAPSED_TIME) - creationTime) >> 5);
	if((t/100)%2)
		t = 100 - (t % 100);
	else
		t = t % 100;
	//printf("%d\n",t);
	return path.getValueAt(t);
}

Vec3f MovingPlatform::getDeltaPos(int lastTime) const
{ //lazy slow hack to get delta position.  there is probably faster way
	int t = speed * ((lastTime - creationTime) >> 5);
	if((t/100)%2)
		t = 100 - (t % 100);
	else
		t = t % 100;
	//printf("%d\n",t);
	return getPos() - path.getValueAt(t);
}

//Vec3f MovingPlatform::getDimensions() const 
//{
//	return Vec3f(width,height,length);
//}

float MovingPlatform::getRadius() const
{
	return radius;
}

Vec3f MovingPlatform::getPathCenter() const
{
    return pathCenter;
}

AABoundingBox MovingPlatform::getAABB() 
{
    aabb.setPosition(getPos());
    return aabb;
}
