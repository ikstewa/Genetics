#include "Force.h"

//-----------------------------------------------------------------------------
// Constructor and Destructor
Force::Force()
{
   force = Vec3f();
   duration = 0;
}
Force::Force(const Vec3f& frc, time_t dur)
{
   force = frc;
   duration = dur;
}
Force::~Force()
{
}

//-----------------------------------------------------------------------------
// Accessor and Mutator for Force.
void Force::setForce(const Vec3f& frc)
{
   force = frc;
}
Vec3f Force::getForce() const
{
   return force;
}

//-----------------------------------------------------------------------------
// Accessor and Mutator for duration.
void Force::setDuration(time_t dur)
{
    duration = dur;
}
time_t Force::getDuration() const
{
   return duration;
}