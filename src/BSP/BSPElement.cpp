#include "BSPElement.h"
#include "math.h"

//-----------------------------------------------------------------------------
// Accessor methods
//void BSPElement::setRadius(float r)
//{
//   radius = r;
//}
float BSPElement::getRadius()
{
    return radius;
}

//void BSPElement::setLocation(Vec3f l)
//{
//   location = l;
//}
Vec3f BSPElement::getLocation()
{
    return location;
}

//void BSPElement::setElement(void *e)
//{
//   element = e;
//}
void* BSPElement::getElement()
{
    return element;
}

int BSPElement::getWeight()
{
    return weight;
}
//void BSPElement::setWeight(int w)
//{
//   weight = w;
//}

//-----------------------------------------------------------------------------
//
//return 0 for on plane, negative number for one side of plane, positive for the other
float BSPElement::elementVsPlane(float loc, int axis)
{
    float ret;

    switch(axis)
    {
        // x
        case 0:
            ret = location.x - loc;
            break;
        // y
        case 1:
            ret = location.y - loc;
            break;
        // z
        case 2:
            ret = location.z - loc;
            break;
        // err
        default:
            break;
    }

    if(fabs(ret) < radius)
        ret = 0;

    return ret;
}
