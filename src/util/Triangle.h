//-----------------------------------------------------------------------------
// Daniel White
// 10/25/08
// 
//
// Description:
//	The PhysicsEntity is either an entity or a triangle. This is what goes in the physicsentity BSP tree
//-----------------------------------------------------------------------------


#ifndef __TRIANGLE__
#define __TRIANGLE__

#include <vector>
#include "vec3.h"
#include "ray.h"

using namespace::std;

class Triangle; //Forward declaration in order to force correct compile.
struct Hit
{	
    const Triangle* triangle;
    float nearIntersect;
    float farIntersect;
  
    Hit()
    {
        triangle = NULL;
        nearIntersect = 0.0;
        farIntersect = 0.0;
    }
	
};

//-----------------------------------------------------------------------------
//
class Triangle
{

public:
    Triangle() {}
    Triangle(vector<Vec3f> v);
    ~Triangle() {}
    Vec3f getLocation() const {return center;}
    float getRadius() const {return radius;}
    vector<Vec3f> getVertices() const {return vertices;}
    bool intersect(const Ray &ray, Hit &hit) const;

    vector<Vec3f> normal;
    vector<Vec3f> uv;
    int texID;

    void setIndex(int i) {index = i;}
    int getIndex() const {return index;}



protected:

    vector<Vec3f> vertices;
    Vec3f center;
    float radius;
    int index;
};

#endif
