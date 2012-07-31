#include <stdlib.h>
#include <iostream>
#include <math.h>
//#include <fstream>
//#include <string>
#include "AABoundingBox.h"
#include <GL/glew.h>

//-----------------------------------------------------------------------------
// Center point of the AA Bounding Box
Vec3f AABoundingBox::getCenter() const
{
    return Vec3f(loc.x, loc.y, loc.z);
}
void AABoundingBox::setPosition(const Vec3f &v)
{
    loc = v;
}

//-----------------------------------------------------------------------------
// Max/min x value
float AABoundingBox::getMaxX() const
{
    return loc.x + halfWidth;
}
float AABoundingBox::getMinX() const
{
    return loc.x - halfWidth;
}

//-----------------------------------------------------------------------------
// Max/min y value
float AABoundingBox::getMaxY() const
{
    return loc.y + halfHeight;
}
float AABoundingBox::getMinY() const
{
    return loc.y - halfHeight;
}

//-----------------------------------------------------------------------------
// Max/min z value
float AABoundingBox::getMaxZ() const
{
    return loc.z + halfDepth;
}
float AABoundingBox::getMinZ() const
{
    return loc.z - halfDepth;
}
//-----------------------------------------------------------------------------
// Max and Min Vec3f accessor
Vec3f AABoundingBox::getMax() const
{
    return Vec3f(getMaxX(), getMaxY(), getMaxZ());
}
Vec3f AABoundingBox::getMin() const
{
    return Vec3f(getMinX(), getMinY(), getMinZ());
}
//-----------------------------------------------------------------------------
// Radius getter
float AABoundingBox::getRadius() const
{
    return sqrt(halfDepth*halfDepth + halfWidth*halfWidth + halfHeight*halfHeight);
}
//-----------------------------------------------------------------------------
// Width getter/setter
float AABoundingBox::getWidth() const
{
    return halfWidth*2.0f;
}
void AABoundingBox::setWidth(float wid)
{
    halfWidth = wid/2.0f;
}

//-----------------------------------------------------------------------------
// Height getter/setter
float AABoundingBox::getHeight() const
{
    return halfHeight*2.0f;
}
void AABoundingBox::setHeight(float hei)
{
    halfHeight = hei/2.0f;
}

//-----------------------------------------------------------------------------
// Depth getter/setter
float AABoundingBox::getDepth() const
{
    return halfDepth*2.0f;
}
void AABoundingBox::setDepth(float dep)
{
    halfDepth = dep/2.f;
}
//-----------------------------------------------------------------------------
// collision test for the XZ coordinate ignoring Y
bool AABoundingBox::intersectsXZ(const AABoundingBox &b) const
{
    // X-coord
    if (getMaxX() >= b.getMinX() && b.getMaxX() >= getMinX())
    {
        // overlapping x-coords
        if (getMaxZ() >= b.getMinZ() && b.getMaxZ() >= getMinZ())
        {
            // overlapping z-coords
            // no need to check y-coords
            return true; // HIT
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// 3d collision test
bool AABoundingBox::intersects(const AABoundingBox &b) const
{
    // X-coord
    if (getMaxX() >= b.getMinX() && b.getMaxX() >= getMinX())
    {
        // overlapping x-coords
        if (getMaxZ() >= b.getMinZ() && b.getMaxZ() >= getMinZ())
        {
            // overlapping z-coords
            if (getMaxY() >= b.getMinY() && b.getMaxY() >= getMinY())
            {
                return true; // HIT
            }
        }
    }

    return false;
}

bool AABoundingBox::intersectRay(Ray ray, Hit &hit)
{
    Vec3f max = getMax();
    Vec3f min = getMin();

    float tNear = std::numeric_limits<float>().min();
    float tFar = std::numeric_limits<float>().max();  
    // check each plane for intersections
    for (int i=0; i < 3; ++i)
    {
        if (ray.d[i] == 0.0)
        {
            if (ray.s[i] < min[i] || ray.s[i] > max[i])
            {
                return false;
            }
        }
        else
        {
            float t1 = (min[i] - ray.s[i])/ray.d[i];
            float t2 = (max[i] - ray.s[i])/ray.d[i];
      
            if (t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }
      
            if (t1 > tNear) tNear = t1;
            if (t2 < tFar) tFar = t2;
            if (tNear > tFar || tFar < 0) return false;
        }
    }
  
    // if we've gotten to this point we've hit
    hit.nearIntersect = tNear;
    hit.farIntersect = tFar;
    return true;
}

//-----------------------------------------------------------------------------
// Draws the BoundingBox
//FIXME: Move this functionality to the RenderManager.
void AABoundingBox::draw()
{
    glColor4f(1.0,0.0,0.0,1.0);
    glPushMatrix();

        glBegin(GL_LINE_STRIP);
            // TOP
            glVertex3f(getMaxX(), getMaxY(), getMaxZ());
            glVertex3f(getMinX(), getMaxY(), getMaxZ());
            glVertex3f(getMinX(), getMaxY(), getMinZ());
            glVertex3f(getMaxX(), getMaxY(), getMinZ());
            glVertex3f(getMaxX(), getMaxY(), getMaxZ());
        glEnd();

        glBegin(GL_LINE_STRIP);
            // BOTTOM
            glVertex3f(getMaxX(), getMinY(), getMaxZ());
            glVertex3f(getMinX(), getMinY(), getMaxZ());
            glVertex3f(getMinX(), getMinY(), getMinZ());
            glVertex3f(getMaxX(), getMinY(), getMinZ());
            glVertex3f(getMaxX(), getMinY(), getMaxZ());
        glEnd();

        glBegin(GL_LINES);
            //SIDES
            glVertex3f(getMaxX(), getMaxY(), getMaxZ());
            glVertex3f(getMaxX(), getMinY(), getMaxZ());

            glVertex3f(getMinX(), getMaxY(), getMaxZ());
            glVertex3f(getMinX(), getMinY(), getMaxZ());

            glVertex3f(getMinX(), getMaxY(), getMinZ());
            glVertex3f(getMinX(), getMinY(), getMinZ());

            glVertex3f(getMaxX(), getMaxY(), getMinZ());
            glVertex3f(getMaxX(), getMinY(), getMinZ());
        glEnd();


    glPopMatrix();
}

