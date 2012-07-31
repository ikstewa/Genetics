#ifndef	__PARTICLE_SYSTEM_H
#define __PARTICLE_SYSTEM_H

#include "ParticleGroup.h"
#include "PDomain.h"
#include "ParticleEffect.h"
#include "ParticleEmitter.h"

//Used to specify default domains
const Vec3f P010(0.0f, 1.0f, 0.0f);
const Vec3f P000(0.0f, 0.0f, 0.0f);
const Vec3f P111(1.0f, 1.0f, 1.0f);

class ParticleSystem {

private:
	ParticleGroup particleGroup;
	float mass; //Used in calculations
	float massStdDev;
	float alpha;
	float alphaStdDev;
	float age;
	float ageStdDev;
    float angularVel;
    float angularVelStdDev;
	float dt;    
    int textId;
	pDomain* position;
	pDomain* velocity;
	pDomain* color;
	pDomain* rot;
	pDomain* size; //Render size
	std::vector<ParticleEffect*> effectList;
	ParticleEmitter* emitter;
	Vec3f pos; //hack!!!
	Vec3f velScale;
	int type;

public:
	

	//Initialize with default values
	ParticleSystem(int numParticles);

	~ParticleSystem();

    //Apply all the user supplied values at once.
	void initialize();

	//TODO: Change this, shouldn't send pointer.
	void setEmitter(ParticleEmitter* emitter);

	void setPosition(const pDomain &domain);

	void setColor(const pDomain &domain);

	void setVelocity(const pDomain &domain);

	void setRot(const pDomain &domain);

	void setSize(const pDomain &domain);

	void setAlpha(float alpha, float stdDev);

	void setAge(float age, float stdDev);

	void setMass(float mass, float stdDev);

    void setAngularVel(float angularVel, float stdDev);
	float getAngularVel();

	void setMaxParticles(size_t maxParticles);

	void setTimeStep(float dt);

    void setTextId(int textId);

    int getTextId();

	size_t getMaxParticles();

    void addEffect(ParticleEffect* effect);


    ParticleList::iterator getParticleListBegin();

    ParticleList::iterator getParticleListEnd();

    int getSize();

    void step();

    float getDt();

    void initializeParticle(Particle* p);

	//Hackish
	void setParticleSysLoc(Vec3f pos);
	Vec3f getParticleSysLoc();
	void setVelScale(Vec3f pos);
	Vec3f getVelScale();

	void setType(int type);
	int getType();
};

#endif