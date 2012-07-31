// Ray
//
// Created by Greg Hoffman
// Copyright 2008
// A basic ray implementation

#ifndef __RAY_H
#define __RAY_H

#include "math.h"
#include "vec3.h"


struct Ray
{	
  Vec3f s; // the start position of the ray
  Vec3f d; // the direction vector
  
  Ray()
  {
    s = Vec3f(0,0,0);
    d = Vec3f(0,0,0);
  }
  
  Ray(Vec3f &start, Vec3f &dir)
  {
    s = start;
    d = dir;
    d.normalize();
  }
  
  Vec3f getHitpoint(float t) const
  {
    return Vec3f(s.x + t*d.x, s.y + t*d.y, s.z + t*d.z);
  }
	
};

#endif