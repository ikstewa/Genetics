#include "ParticleEmitter.h"
#include "ParticleSystem.h"

ParticleEmitter::ParticleEmitter(const pDomain &domain, int rate, const Particle &p, bool oneTimeEmitter) {
    this->domain = domain.copy();
    this->rate = rate;
	this->p = new Particle(p);
	this->oneTimeEmitter = oneTimeEmitter;
	this->done = false;
}

void ParticleEmitter::emit(ParticleGroup &particleGroup) {
	int maxParticles = (int)particleGroup.getMaxParticles();
    int size = (int)particleGroup.size();
    int numToGenerate;
    int i;
    Particle *p;

    if (size >= maxParticles) { //Already reached the max number of particles
        return;
    }

    numToGenerate = size + rate > maxParticles ? maxParticles - size : rate; 

    for (i=0; i<numToGenerate; i++) {
		p = new Particle(*this->p);
		p->pos += this->domain->Generate();
        particleGroup.add(p);
    }
}

void ParticleEmitter::emit(ParticleSystem *pSys, ParticleGroup &particleGroup) {
	int maxParticles = (int)pSys->getMaxParticles();
    int size = (int)particleGroup.size();
    int numToGenerate;
    int i;
    Particle *p;

    if (size >= maxParticles) { //Already reached the max number of particles
		if (oneTimeEmitter) done = true;
        return;
	}else if (done) {
		return;
	}

    numToGenerate = size + rate > maxParticles ? maxParticles - size : rate; 

    for (i=0; i<numToGenerate; i++) {
        p = new Particle();
        pSys->initializeParticle(p);

		p->pos = domain->Generate() + this->p->pos;
        //p->pos = position->Generate(); //Particle starts at the emitter position.
        particleGroup.add(p);
    }
}

void ParticleEmitter::addEffect(ParticleEffect* effect) {
	effectList.push_back(effect->copy());
}

void ParticleEmitter::update(float dt) {
	int numEffects = (int)effectList.size();
    
    //Apply the effects
    for (int i=0; i<numEffects; i++) {
		effectList[i]->execute(*this->p, dt);
    }

	this->p->pos += this->p->vel * dt;
}

//TODO: Implement correctly
ParticleEmitter* ParticleEmitter::copy() {
	//ParticleEmitter* emitter = new ParticleEmitter(*this->domain, this->rate, this->p);
	//return emitter;
	return NULL;
}
