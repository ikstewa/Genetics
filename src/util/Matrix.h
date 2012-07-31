/*
 *  Matrix.h
 *  RayTracer
 *
 *  Created by Greg Hoffman on 10/7/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MATRIX
#define MATRIX

#include "vec3.h"
#include <stdlib.h>
//#include <GL/glew.h>

class Matrix
{
public:
  float d[4][4];
  
  Matrix()
  {
    Zero();
  }
  
  Matrix(float v[4][4]);
  Matrix(const float v[16]);//reads in column major
  
  // create a rotational matrix inthe given axis
  void RotX(float xrot);
  void RotY(float yrot);
  void RotZ(float zrot);
  void Scale(Vec3d s);
  void Scale(Vec3f s);
  
  // rotation about an arbitrary axis
  void AxisRot(Vec3d xaxis, Vec3d yaxis, Vec3d zaxis);
  void AxisRot(Vec3f xaxis, Vec3f yaxis, Vec3f zaxis);
  
  Matrix operator+(const Matrix &v) const;
  Matrix operator*(const Matrix &v) const;
  Matrix operator*(const float v) const;
  Vec3d operator*(const Vec3d &v) const;
  Vec3f operator*(const Vec3f &v) const;
  
  void MakeIdentity();
  void Zero();
  void Transpose();
  void Invert();
  void Transform(Vec3d &v);
  void Transform(Vec3f &v);

  void asArray(float data[]) const;
  void getRow(int idx, float[4]) const;
};

#endif