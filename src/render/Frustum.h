/**
 * An attempt to rewrite culling. Ideas from gametutorials.com
 */

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "../util/vec3.h"

class Frustum
{
public:

    // updates the frustrum values, call every screen render
    void CalculateFrustum();

    // Hit check a 3D point
    //bool PointInFrustum(float x, float y, float z);

    // Hit check a sphere
    bool SphereInFrustum(Vec3f*, float);

    // Hit check a cube
    //bool CubeInFrustum(float x, float y, float z, float size);



    // Holds each side of the frustum
    // RIGHT, LEFT, BOTTOM, TOP, BACK, FRONT
    // X, Y, Z, dist to orig
    float m_Frustum[6][4];

protected:

    void NormalizePlane(int idx);
};

#endif
