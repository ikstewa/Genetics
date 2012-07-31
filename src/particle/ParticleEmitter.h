#ifndef	__PARTICLE_EMITTER_H
#define __PARTICLE_EMITTER_H

#include "PDomain.h"
#include "ParticleEffect.h"

class ParticleSystem;
class ParticleGroup;

class ParticleEmitter{
public:
    pDomain* domain;
    int rate;
	Particle* p;
	std::vector<ParticleEffect*> effectList;
	bool oneTimeEmitter;
	bool done;

    //Rate specifies the maximum number of particles generated every frame.
    ParticleEmitter(const pDomain &domain, int rate, const Particle &p, bool oneTimeEmitter = false);

	//initalize the particles with the ParticleSystem state
	void emit(ParticleSystem *pSys, ParticleGroup &particleGroup); 

	//initalize the particles with the Emitter's state
	void emit(ParticleGroup &particleGroup);

	void addEffect(ParticleEffect* effect);

	void update(float dt);

	ParticleEmitter* copy();
};

#endif

