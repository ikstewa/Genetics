/*
	Orb
	The base orb class

	Created By: Ryan Schmitt
*/

#ifndef _ORB_H_
#define _ORB_H_

#define ORB_TYPE_ADD_TIME   1
#define ORB_TYPE_SLOW_TIME  2
#define ORB_TYPE_INCR_SPD   3
#define ORB_TYPE_INCR_JUMP  4
#define ORB_TYPE_ABILITY    5
#define ORB_TYPE_FINAL_ORB  6

#include "RenderedEntity.h"
using namespace::std;

class Orb : public RenderedEntity
{
public:
    Orb();
    ~Orb();
	void hitEntity(Entity* entity);
    //void hitTerrain(const Vec3f& dir, const Vec3f& hitPoint, const Vec3f& aabbHitPoint);
    void setOrbType(int t) {orbType = t;}
    int getOrbType() const {return orbType;}
    void setOrbTypeData(float d) {typeData = d;}
    float getOrbTypeData() const {return typeData;}

private:
    int orbType;
    float typeData;
};

#endif