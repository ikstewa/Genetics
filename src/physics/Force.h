//-----------------------------------------------------------------------------
// Ryan Schmitt
// 10/14/08
// 
//
// Description:
//	The Force object models a force acting on any game Entity.
//-----------------------------------------------------------------------------


#ifndef __GENETICS_FORCE__
#define  __GENETICS_FORCE__

#include "../util/vec3.h"

//-----------------------------------------------------------------------------
//
class Force
{

public:
   Force();
   Force(const Vec3f& frc, time_t dur);
   ~Force();

   void setForce(const Vec3f& frc);
   Vec3f getForce() const;
   void setDuration(time_t dur);
   time_t getDuration() const;

protected:
   Vec3f force;
   time_t duration;

private:


};

#endif
