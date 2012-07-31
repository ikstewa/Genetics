/*
 *  Curve.h
 *  Animation Curve. Defines the templat eclass that allows us to abstract 
 *  between 1d, 2d, and 3d curves in space. This helps with the keyframing of
 *  attributes as well
 *
 *  Created by Greg Hoffman on 6/21/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CURVE_H
#define CURVE_H

#include <vector>
#include "Matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

template <class base>
class Curve
{
public:
  
  /**
   Curve type
   What type of curve are we representing
   */
  
  enum CurveType { LINEAR, BEZIER, CUBIC };
  
  
  /**
   Curve
   The base curve takes a base value to return.
   */
  Curve(base v, CurveType t = BEZIER){baseReturn = v; type = t;}
  
  Curve() { type = BEZIER; }
  
  void setBaseReturn(base v) { baseReturn = v; }
  void setCurveType(CurveType t) { type = t; }
  
  void AddPoint(base p, base ic, base op, int t);
  base getValueAt(int time) const;
  base getValueAt(float ta) const;
  int getNumPoints(){ return points.size(); }
  base getPointAt(int i) { return points[i].pos; }
  base getInAt(int i) { return points[i].inCtrl; }
  base getOutAt(int i) { return points[i].outCtrl; }
  CurveType getCurveType() { return type; }
  
private:
  struct ControlPoint
  {
    ControlPoint(base p, base ic, base op, int t = 0)
    {
      pos = p; outCtrl = op; inCtrl = ic; time = t;
    }
    base pos;
    base outCtrl;
    base inCtrl;
    int time;
  };
  
  base baseReturn;
  int startFrame;
  int endFrame;
  CurveType type;
  std::vector<ControlPoint> points;
};

template <class base>
base Curve<base>::getValueAt(int time) const
{
  unsigned int i; 
  for (i=0; i < points.size(); i++ )
  {
    if ( time < points[i].time)
      break;
  }
  
  // check to see if we went out of range
  if ( i >= points.size() || i == 0 )
  {
    // if no elements return baseReturn
    if ( points.size() == 0 )
      return baseReturn;
    else if (i > 0)
      return points[i-1].pos;
    else
      return points[i].pos;
  }
  
  // if we didn't go out of range, lets compute our value given the default
  // interpretation type
  int ct = time - points[i-1].time;
  float t = (float)ct / (float)( points[i].time - points[i-1].time);
  
  base final;
  if ( type == BEZIER )
  {
    final = points[i-1].pos*((1-t)*(1-t)*(1-t))
    + points[i-1].outCtrl*(3*t*(1-t)*(1-t))
    + points[i].inCtrl*(3*t*t*(1-t))
    + points[i].pos*t*t*t;
  }
  else if ( type == LINEAR )
  {
    final = points[i-1].pos + (points[i].pos - points[i-1].pos)*t;
  }
  else if ( type == CUBIC )
  {
    base n0, n1, n2, n3;
    if ( (i-1) == 0 )
      n0 = n1 = points[i-1].pos;
    else
    {
      n1 = points[i-1].pos;
      n0 = points[i-2].pos;
    }
    
    if ( (i + 1) > points.size() -1 )
      n3 = n2 = points[i].pos;
    else
    {
      n2 = points[i].pos;
      n3 = points[i+1].pos;
    }
    
    base t1 = (n1 - n0)*0.5 + (n2 - n1)*0.5;
    base t2 = (n2 - n1)*0.5 + (n3 - n2)*0.5;
    
    final = n1*(2*t*t*t - 3*t*t + 1) +
    n2*(3*t*t - 2*t*t*t) +
    t1*(t*t*t - 2*t*t + t) + 
    t2*(t*t*t - t*t);
    
  }
  
  // return our value
  return final;
}

template <class base>
base Curve<base>::getValueAt(float ta) const
{
  // check to make sure we aren't polling a zero size
  if (points.size() == 0)
  {
    return baseReturn;
  }

  int time = (int)(points[points.size()-1].time * ta);
  unsigned int i; 
  for (i=0; i < points.size(); i++ )
  {
    if ( time < points[i].time)
      break;
  }
  
  if ( !(i < points.size()) )
  {
    i = (unsigned int)(points.size() -1);
  }
  
  // check to see if we went out of range
  if ( i >= points.size() || i == 0 )
  {
    // if no elements return baseReturn
    if ( points.size() == 0 )
      return baseReturn;
    else if (points.size() == 1 || i == 0)
      return points[0].pos;
    else
      return points[i-1].pos;
  }
  
  // if we didn't go out of range, lets compute our value given the default
  // interpretation type
  int ct = time - points[i-1].time;
  //float t = (float)ct / (float)( points[i].time - points[i-1].time);
  float t = points[i-1].time / (float)points[points.size()-1].time;
  t = (ta - t) / (float)((points[i].time - points[i-1].time) / (float)points[points.size()-1].time);
  
  base final;
  if ( type == BEZIER )
  {
    final = points[i-1].pos*((1-t)*(1-t)*(1-t))
    + points[i-1].outCtrl*(3*t*(1-t)*(1-t))
    + points[i].inCtrl*(3*t*t*(1-t))
    + points[i].pos*t*t*t;
  }
  else if ( type == LINEAR )
  {
    final = points[i-1].pos + (points[i].pos - points[i-1].pos)*t;
  }
  else if ( type == CUBIC )
  {
    base n0, n1, n2, n3;
    if ( (i-1) == 0 )
      n0 = n1 = points[i-1].pos;
    else
    {
      n1 = points[i-1].pos;
      n0 = points[i-2].pos;
    }
    
    if ( (i + 1) > points.size() -1 )
      n3 = n2 = points[i].pos;
    else
    {
      n2 = points[i].pos;
      n3 = points[i+1].pos;
    }
    
    base t1 = (n1 - n0)*0.5 + (n2 - n1)*0.5;
    base t2 = (n2 - n1)*0.5 + (n3 - n2)*0.5;
    
    final = n1*(2*t*t*t - 3*t*t + 1) +
    n2*(3*t*t - 2*t*t*t) +
    t1*(t*t*t - 2*t*t + t) + 
    t2*(t*t*t - t*t);
  }
  
  // return our value
  return final;
}


template <class base>
void Curve<base>::AddPoint(base p, base ic, base op, int t)
{
  points.push_back(ControlPoint(p, ic, op, t));
  // resort
  
}

#endif