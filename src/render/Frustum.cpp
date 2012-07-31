
#include "Frustum.h"
#include "../util/Matrix.h"
#include <GL/glut.h>


void Frustum::CalculateFrustum()
{
    GLfloat pMatrix[16];
    GLfloat mMatrix[16];
    Matrix M;

    glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, mMatrix);

    M = Matrix(pMatrix) * Matrix(mMatrix);

    // grab the clipping planes
    // RIGHT, LEFT, BOTTOM, TOP, BACK, FRONT
    // X, Y, Z, dist to orig  (a,b,c,d)

    // RIGHT
    m_Frustum[0][0] = M.d[3][0] - M.d[0][0];
    m_Frustum[0][1] = M.d[3][1] - M.d[0][1];
    m_Frustum[0][2] = M.d[3][2] - M.d[0][2];
    m_Frustum[0][3] = M.d[3][3] - M.d[0][3];

    // LEFT
    m_Frustum[1][0] = M.d[0][0] + M.d[3][0];
    m_Frustum[1][1] = M.d[0][1] + M.d[3][1];
    m_Frustum[1][2] = M.d[0][2] + M.d[3][2];
    m_Frustum[1][3] = M.d[0][3] + M.d[3][3];

    // BOTTOM
    m_Frustum[2][0] = M.d[1][0] + M.d[3][0];
    m_Frustum[2][1] = M.d[1][1] + M.d[3][1];
    m_Frustum[2][2] = M.d[1][2] + M.d[3][2];
    m_Frustum[2][3] = M.d[1][3] + M.d[3][3];

    // TOP
    m_Frustum[3][0] = M.d[3][0] - M.d[1][0];
    m_Frustum[3][1] = M.d[3][1] - M.d[1][1];
    m_Frustum[3][2] = M.d[3][2] - M.d[1][2];
    m_Frustum[3][3] = M.d[3][3] - M.d[1][3];

    // NEAR
    m_Frustum[4][0] = M.d[2][0] + M.d[3][0];
    m_Frustum[4][1] = M.d[2][1] + M.d[3][1];
    m_Frustum[4][2] = M.d[2][2] + M.d[3][2];
    m_Frustum[4][3] = M.d[2][3] + M.d[3][3];

    // FAR
    m_Frustum[5][0] = M.d[3][0] - M.d[2][0];
    m_Frustum[5][1] = M.d[3][1] - M.d[2][1];
    m_Frustum[5][2] = M.d[3][2] - M.d[2][2];
    m_Frustum[5][3] = M.d[3][3] - M.d[2][3];

    for ( int i = 0; i < 6; i++)
        NormalizePlane(i);

}

bool Frustum::SphereInFrustum(Vec3f *pos, float radius)
{
    for( int i = 0; i < 5; i++ )
    {
        if (m_Frustum[i][0] * pos->x + m_Frustum[i][1] * pos->y + m_Frustum[i][2] * pos->z + m_Frustum[i][3] <= -radius)
        {
            return false;
        }
    }

    // passed all planes
    return true;
}

void Frustum::NormalizePlane(int idx)
{
    float mag = (float)sqrt(m_Frustum[idx][0] * m_Frustum[idx][0] +
                            m_Frustum[idx][1] * m_Frustum[idx][1] +
                            m_Frustum[idx][2] * m_Frustum[idx][2] );

    m_Frustum[idx][0] /= mag;
    m_Frustum[idx][1] /= mag;
    m_Frustum[idx][2] /= mag;
    m_Frustum[idx][3] /= mag;

}