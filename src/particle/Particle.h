#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "../util/vec3.h"

struct Particle {
	Vec3f pos;
	Vec3f vel;	
	Vec3f size;    
    Vec3f rot;
	Vec3f color;    
	float alpha;
	float age;
	float mass;
    int texId;
    float angularVel;

	Particle(Vec3f vel, Vec3f pos, Vec3f color, Vec3f size, Vec3f rot, 
            float alpha, float age, float mass, float angularVel);

	Particle(const Particle& p);

	Particle();

    void getBillboardedCoords(Vec3f* pts, float modelview[16]);
};

#endif