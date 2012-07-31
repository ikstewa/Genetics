#ifndef __AA_BOUNDING_BOX_H__
#define __AA_BOUNDING_BOX_H__

#include "vec3.h"
#include "ray.h"
#include "Triangle.h" //for Hit struct
#include <limits>

// Axis Aligned BoundingBox
class AABoundingBox
{

public:

    AABoundingBox() {};
    AABoundingBox( Vec3f in_loc,
                  float in_width, float in_height, float in_depth) :
        loc(in_loc), halfWidth(in_width/2),
        halfHeight(in_height/2), halfDepth(in_depth/2) {}

    ~AABoundingBox() {};

    Vec3f getCenter() const;
    void setPosition(const Vec3f&);
    float getMaxX() const;
    float getMinX() const;
    float getMaxY() const;
    float getMinY() const;
    float getMaxZ() const;
    float getMinZ() const;
    Vec3f getMax() const;
    Vec3f getMin() const;

    float getWidth() const;
    void setWidth(float);
    float getHeight() const;
    void setHeight(float);
    float getDepth() const;
    void setDepth(float);
    float getRadius() const;

    // collision test for the XZ coordinate ignoring Y
    bool intersectsXZ(const AABoundingBox&) const;
    bool intersects(const AABoundingBox&) const;
    bool intersectRay(Ray ray, Hit &hit);

    void draw();

protected:

    Vec3f loc;

    float halfWidth;
    float halfHeight;
    float halfDepth;

};

#endif
