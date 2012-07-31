/*
 *  Matrix.cpp
 *  RayTracer
 *
 *  Created by Greg Hoffman on 10/7/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Matrix.h"

//reads in column major
Matrix::Matrix(const float v[16])
{
  for ( int i=0; i < 4; i++ )
    for ( int j=0; j < 4; j++ )
      d[i][j] = (float)v[(j << 2) | i];
}

Matrix::Matrix(float v[4][4])
{
  for ( int i=0; i < 4; i++ )
    for ( int j=0; j < 4; j++ )
      d[i][j] = v[i][j];
}

void Matrix::Scale(Vec3f s)
{
  Zero();
  d[3][3] = 1;
  d[0][0] = s.x;
  d[1][1] = s.y;
  d[2][2] = s.z;
}

void Matrix::Scale(Vec3d s)
{
  Zero();
  d[3][3] = 1;
  d[0][0] = (float)s.x;
  d[1][1] = (float)s.y;
  d[2][2] = (float)s.z;
}

void Matrix::RotX(float xrot)
{
  Zero();
  d[0][0] = d[3][3] = 1;
  d[1][1] = d[2][2] = cos(xrot);
  d[2][1] = sin(xrot);
  d[1][2] = -d[2][1];
}

void Matrix::RotY(float yrot)
{
  Zero();
  d[0][0] = d[2][2] = cos(yrot);
  d[1][1] = d[3][3] = 1;
  d[0][2] = sin(yrot);
  d[2][0] = -d[0][2];
}

void Matrix::RotZ(float zrot)
{
  Zero();
  d[0][0] = d[1][1] = cos(zrot);
  d[1][0] = sin(zrot);
  d[0][1] = -d[1][0];
  d[2][2] = d[3][3] = 1;
}

void Matrix::AxisRot(Vec3d xaxis, Vec3d yaxis, Vec3d zaxis)
{
  Zero();
  d[0][0] = (float)xaxis.x, d[0][1] = (float)xaxis.y, d[0][2] = (float)xaxis.z;
  d[1][0] = (float)yaxis.x, d[1][1] = (float)yaxis.y, d[1][2] = (float)yaxis.z;
  d[2][0] = (float)zaxis.x, d[2][1] = (float)zaxis.y, d[2][2] = (float)zaxis.z;
  d[3][3] = 1;
  Transpose();
}

void Matrix::AxisRot(Vec3f xaxis, Vec3f yaxis, Vec3f zaxis)
{
  Zero();
  d[0][0] = xaxis.x, d[0][1] = xaxis.y, d[0][2] = xaxis.z;
  d[1][0] = yaxis.x, d[1][1] = yaxis.y, d[1][2] = yaxis.z;
  d[2][0] = zaxis.x, d[2][1] = zaxis.y, d[2][2] = zaxis.z;
  d[3][3] = 1;
  Transpose();
}

Vec3d Matrix::operator*(const Vec3d &v) const
{
  Vec3d temp;
  temp.x = v.x*d[0][0] + v.y*d[0][1] + v.z*d[0][2] + d[0][3];
  temp.y = v.x*d[1][0] + v.y*d[1][1] + v.z*d[1][2] + d[1][3];
  temp.z = v.x*d[2][0] + v.y*d[2][1] + v.z*d[2][2] + d[2][3];
  return temp;
}

Vec3f Matrix::operator*(const Vec3f &v) const
{
  Vec3f temp;
  temp.x = v.x*d[0][0] + v.y*d[0][1] + v.z*d[0][2] + d[0][3];
  temp.y = v.x*d[1][0] + v.y*d[1][1] + v.z*d[1][2] + d[1][3];
  temp.z = v.x*d[2][0] + v.y*d[2][1] + v.z*d[2][2] + d[2][3];
  return temp;
}

void Matrix::MakeIdentity()
{
  Zero();
  d[0][0] = d[1][1] = d[2][2] = d[3][3] = 1;
}

void Matrix::Transform(Vec3f &v)
{
  MakeIdentity();
  d[0][3] = v.x;
  d[1][3] = v.y;
  d[2][3] = v.z;
}

void Matrix::Transform(Vec3d &v)
{
  MakeIdentity();
  d[0][3] = (float)v.x;
  d[1][3] = (float)v.y;
  d[2][3] = (float)v.z;
}

void Matrix::Zero()
{
  for ( int i=0; i < 4; i++ )
    for ( int j=0; j < 4; j++ )
      d[i][j] = 0;
}
// 0 1 2 3
// 0 1 2 3
// 0 1 2 3
// 0 1 2 3
// This is not the inverse!
// Changed method name from inverse to transpose.
void Matrix::Transpose()
{
  for ( int i=0; i < 4; i++ )
    for ( int j=i+1; j < 4; j++ )
    {
      float temp = d[i][j];
      d[i][j] = d[j][i];
      d[j][i] = temp;
    }
  
}

//Matrix Matrix::operator+(const Matrix &v) const
//{
//  Matrix temp;
//  for ( int i=0; i < 4; i++ )
//    for ( int j=0; j < 4; j++ )
//      temp.d[i][j] += v.d[i][j];
//  
//  return temp;
//}

Matrix Matrix::operator*(const Matrix &v) const
{
  Matrix temp;
  float sum = 0;
  for ( int i=0; i < 4; i++ )
  {
    for ( int j=0; j < 4; j++ )
    {
      sum = 0;
      for ( int k=0; k < 4; k++ )
        sum += d[i][k] * v.d[k][j];
      temp.d[i][j] = sum;
    }
  }
  return temp;
}

Matrix Matrix::operator+(const Matrix &v) const
{
  Matrix temp;
  float sum = 0;
  for ( int i=0; i < 4; i++ )
  {
    for ( int j=0; j < 4; j++ )
    {
      temp.d[i][j] = d[i][j] + v.d[i][j];
    }
  }
  return temp;
}

Matrix Matrix::operator*(const float v) const
{
  Matrix temp;
  float sum = 0;
  for ( int i=0; i < 4; i++ )
  {
    for ( int j=0; j < 4; j++ )
    {
      temp.d[i][j] = d[i][j] * v;
    }
  }
  return temp;
}

void Matrix::Invert()
{
  
}

void Matrix::asArray(float data[16]) const
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            (float)data[(j << 2) | i] = d[i][j];
        }
    }
}

void Matrix::getRow(int idx, float v[4]) const
{
    v[0] = d[idx][0];
    v[1] = d[idx][1];
    v[2] = d[idx][2];
    v[3] = d[idx][3];
}
