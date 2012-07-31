//-----------------------------------------------------------------------------
// GlutWindow.h
// 10/3/08
//
// Ian Stewart
// istewart@calpoly.edu
//
// Camara defined by point, direction, and up vector.
// With this setup we can have methods to move camera based on view direction or look-at-point.
// Eventually I'd like to have some sort of animation tweening with the Camera for smooth camera movement
//
//-----------------------------------------------------------------------------

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../util/vec3.h"

class Camera {

   public:
      
      float fov;
      float zNear, zFar;

      Camera(  float setFOV, float setX, float setY, float setZ,
               float dx, float dy, float dz,
               //float upx, float upy, float upz,
               float setzNear, float setzFar)
      {
         loc = Vec3f(setX, setY, setZ);
         dir = Vec3f(dx, dy, dz);
         up = Vec3f(0, 1, 0);//Vec3<float>(upx, upy, upz);
         fov = setFOV;
         zNear = setzNear;
         zFar = setzFar;
      }

      virtual Vec3f getLookAt() const
      {
         return loc + dir;
      }

      virtual Vec3f getLoc() const
      {
         return loc;
      }

      virtual Vec3f getDir() const
      {
         return dir;
      }

      virtual Vec3f getUp() const
      {
         return up;
      }

protected:
      Vec3f loc;
      Vec3f dir;
      Vec3f up;



};

#endif