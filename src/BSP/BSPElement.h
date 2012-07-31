#ifndef __BSPELEMENT_H__
#define __BSPELEMENT_H__

#include "../util/vec3.h"

class BSPElement
{
public:
    BSPElement() {};

    BSPElement(void *e, int w, Vec3f l, float r) :
        element(e), weight(w), location(l), radius(r) {};

    //----------------------------
    // Accessor methods
    //void setRadius(float r);
    float getRadius();

    //void setLocation(Vec3f l);
    Vec3f getLocation();

    //void setElement(void *e);
    void *getElement();

    int getWeight();
    //void setWeight(int w);


    //return 0 for on plane, negative number for one side of plane, positive for the other
    float elementVsPlane(float loc, int axis);

private:
    void *element;
    int weight;
    Vec3f location;
    float radius;
};

#endif
