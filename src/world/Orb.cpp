#include "Orb.h"

//-----------------------------------------------------------------------------
// Constructor and Destructor
Orb::Orb()
{
	setType(ENTITY_TYPE_ORB);
}
Orb::~Orb()
{
}

//-----------------------------------------------------------------------------
// Hit Detection
void Orb::hitEntity(Entity* entity)
{
   //TODO: entity interaction
   //Player is handling this as of now.
}
/*
void hitTerrain(const Vec3f& dir, const Vec3f& hitPoint, const Vec3f& aabbHitPoint)
{
//    //TODO: terrain interaction
}
*/