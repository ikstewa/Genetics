// VECTOR
// 
// Created by Greg Hoffman
// Copyright 2008
// A basic vector implementation that allows you to do vector math

#ifndef __VEC3_H
#define __VEC3_H

#ifndef NULL
#define NULL 0L
#endif

#include "math.h"

template <class base>
struct Vec3
{
  base x; // the x component of the vector
  base y; // the y component of the vector
  base z; // the z component of the vector

  // constructors
  Vec3() { x = y = z = 0; }
  Vec3(base X, base Y, base Z) { x=X; y=Y; z=Z;}
  
  // assignment operator
  const Vec3<base> &operator= (const Vec3<base> &vec) {x=vec.x; y=vec.y; z=vec.z; return *this;}
  const int operator== (const Vec3<base> &vec) const { return (this->x == vec.x && this->y == vec.y && this->y == vec.y && this->x == vec.x); }

  // basic add operator
  Vec3<base> operator+ (const Vec3<base> &add) const;
  const Vec3<base> &operator+= (const Vec3<base> &add){ *this = *this + add; return *this; }
  
  // the subtract operator
  Vec3<base> operator- (const Vec3<base> &s) const; 
  const Vec3<base> &operator-= (const Vec3<base> &sub) { *this = *this - sub; return *this;}
  
  // scalar multiply
  Vec3<base> operator* (const base &val) const;
  Vec3<base> operator*= (const base &val) { x *= val; z *= val; y *= val; return *this;}

  //component wise multiply
  Vec3<base> operator* (const Vec3<base> &val) { Vec3<base> temp; temp.x = val.x * x; temp.y = y * val.y; temp.z *= z * val.z; return temp;}
  Vec3<base> operator*= (const Vec3<base> &val) { x *= val.x; y *= val.y; z *= val.z; return *this;}
  
  // scalar divide
  Vec3<base> operator/ (const base &val) const;// Scalar Divide
  Vec3<base> operator/= (const base &val) { x /= val; z /= val; y /= val; return *this;}
  
  // square of the length
  base squareLength() { return (x*x + y*y + z*z);}

  // assingment
  base &operator[](const int &val);

  // magnitude or length
  base length() { return (sqrt(squareLength()));}
  Vec3<base> normal() const;             // returns the normal of the vector
  void normalize();                      // In place Normalization
  base dot(const Vec3<base> &v) const;   // Dot Product
  Vec3<base> cross(const Vec3<base> &v) const ; // Cross Product
  base angle(const Vec3<base> &v);       // returns the angle between 2 vec
  void abs();                            // sets the vector to absoulute
  void zero();                           // Zeros the Vector
  Vec3<base> getSurfNorm(Vec3<base> p1, Vec3<base> p2) const;
	
};
template<class base>
base &Vec3<base>::operator[](const int &val)
{
  if (val == 0)
  {
    return x;
  }
  else if (val == 1)
  {
    return y;
  }
  else
  {
    return z;
  }
}

template<class base>
Vec3<base> Vec3<base>::getSurfNorm(Vec3<base> p1, Vec3<base> p2) const
{
  Vec3<base> a;
  Vec3<base> b;
  Vec3<base> result;

  a.x = x - p1.x;
  a.y = y - p1.y;
  a.z = z - p1.z;

  b.x = x - p2.x;
  b.y = y - p2.y;
  b.z = z - p2.z;

  result.x = a.y * b.z - b.y * a.z;
  result.y = b.x * a.z - a.x * b.z;
  result.z = a.x * b.y - b.x * a.y;
  result.normalize();
  return result;
}
template<class base>
Vec3<base> Vec3<base>::operator-(const Vec3<base> &s) const
{
  return Vec3<base>(x - s.x, y - s.y, z - s.z);
}
template<class base>
Vec3<base> Vec3<base>::operator/(const base &val) const
{
  return Vec3<base>(x / val, y / val, z / val);
}

template<class base>
Vec3<base> Vec3<base>::normal() const
{
  Vec3<base> temp = *this;
  base lengthT = temp.length();
  if (lengthT > 0)
    return (temp/lengthT);
  else
    return temp;
}
template<class base>
void Vec3<base>::normalize()
{
  base lengthT = length();
  if(lengthT > 0)
      *this = (*this/lengthT);
}

template<class base>
base Vec3<base>::dot(const Vec3<base> &v) const
{
  return (x*v.x + y*v.y + z*v.z);
}

template<class base>
Vec3<base> Vec3<base>::cross(const Vec3<base> &v) const
{
  return Vec3<base>(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

template<class base>
base Vec3<base>::angle(const Vec3<base> &v)
{
  base crossProd = dot(v);
  base combinedMag = length();
  combinedMag *= v.length();
  return (acos(crossProd/combinedMag));
}

template<class base>
void Vec3<base>::abs()
{
  if ( x < 0 )
    x = -x;
  if ( y < 0 )
    y = -y;
  if ( z < 0 )
    z = -z;
}

template<class base>
void Vec3<base>::zero()
{
  x = y = z = 0;
}

template<class base>
Vec3<base> Vec3<base>::operator+( const Vec3<base> &add ) const
{
  return Vec3<base>(x + add.x, y + add.y, z + add.z); 
}

template<class base>
Vec3<base> Vec3<base>::operator*(const base &val) const
{
  return Vec3<base>(val*x, val*y, val*z); 
}

typedef Vec3<double> Vec3d;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
#endif