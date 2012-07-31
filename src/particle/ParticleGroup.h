#ifndef	__PARTICLEGROUP_H
#define __PARTICLEGROUP_H

#include "Particle.h"
#include <vector>

typedef std::vector<Particle*> ParticleList;

class ParticleGroup {
private:
	ParticleList particles;
	size_t maxParticles;

public:
	ParticleGroup() {
		maxParticles = 0;
	}

	ParticleGroup(const ParticleGroup& pGrp): 
			maxParticles(pGrp.maxParticles), particles(pGrp.particles) {
	}

	ParticleGroup(size_t max) : maxParticles(max) {
		particles.reserve(max);
	}

	~ParticleGroup() {
		while (particles.size() > 0) {
			delete &particles[particles.size()-1];
			particles.pop_back();
		}
	}

	ParticleGroup &operator=(const ParticleGroup &pGrp) {
		if (this != &pGrp) {
			particles = pGrp.particles;
			maxParticles = pGrp.maxParticles;
		}
		
		return *this;
	}

	size_t size() {
		return particles.size();
	}

	bool add(Particle *p) {
		if (particles.size() < maxParticles) {
			particles.push_back(p);
			return true;
		}

		return false;
	}

	ParticleList::iterator remove(ParticleList::iterator it) {
		delete (*it);
		return particles.erase(it);
	}

	ParticleList::iterator begin() {
		return particles.begin();
	}

	ParticleList::iterator end() {
		return particles.end();
	}

    ParticleList& getParticleList() {
        return particles;
    }

	size_t getMaxParticles() {
		return maxParticles;
	}

	void setMaxParticles(size_t maxParticles) {
		this->maxParticles = maxParticles;
		if (particles.size() > maxParticles) {
			particles.resize(maxParticles);
		}else {
			particles.reserve(maxParticles);
		}
	}
};

#endif