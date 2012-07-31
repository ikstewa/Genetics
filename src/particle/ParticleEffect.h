#ifndef	__PARTICLE_EFFECT_H
#define __PARTICLE_EFFECT_H
#include "ParticleGroup.h"
#include "PDomain.h"

class ParticleSystem;
class ParticleEmitter;

class ParticleEffect {
public:
	virtual ~ParticleEffect() {}
	virtual void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) = 0;
	virtual void execute(Particle &p, float dt) = 0;
	virtual ParticleEffect* copy() = 0;
};


class Gravity: public ParticleEffect {
private:
	Vec3f dir;

public:
	Gravity(Vec3f dir);
    
    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};


//Particles will fade to 0 alpha between time-sigma and time+sigma
class Fade: public ParticleEffect {
private:
	float fadeRate;
	float sigma;

public:
    //fadeRate specifies the amount the alpha is reduced each step, sigma provides a certain amount
    //of randomness to the rate. 
	Fade(float fadeRate, float sigma=0);

	void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

//bounce

//fountain


//emitter
/*
class Emitter: public ParticleEffect {
private:
    pDomain* position;
    int rate;

public:
    //Rate specifies the number of particles generated every time execute is called.
    Emitter(const pDomain &position, int rate);

    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
    ParticleEffect* copy();
};*/


class KillAge: public ParticleEffect {
private:
    float maxAge;
    float sigma;

public:
    KillAge(float maxAge, float sigma);

    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

class QuadraticDrag: public ParticleEffect {
private:
    float drag;
	float increment;

public:
    QuadraticDrag(float drag, float increment);

    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

class LinearDrag: public ParticleEffect {
private:
    float drag;

public:
    LinearDrag(float drag);

    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

class Friction: public ParticleEffect {
private:
    float friction;

public:
    Friction(float friction);

    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

//Specify a zone within which particles are accelerated by the dirDomain. default to effect objects within the domain.
class AccelerateZone: public ParticleEffect {
private:
    pDomain* dirDomain;
    pDomain* zone;
    bool within;

public:
    AccelerateZone(pDomain &dirDomain, pDomain &zone, bool within=true);
    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

//Particles within the zone are given an attractive force towards the point. 
//If within == false, the force effects particles outside the zone.
//The force applied is inversely proportional to the square of the distance from the particle to the point.
class AttractZone: public ParticleEffect {
private:
    Vec3f point;
    pDomain* zone;
    bool within;
    float power;
    float epsilon;

public:
    AttractZone(Vec3f &point, pDomain &zone, float power, float epsilon=100, bool within=true);
    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};


//Particles within the zone are given an attractive force away from the point. 
//If within == false, the force effects particles outside the zone.
class RepelZone: public ParticleEffect {
private:
    Vec3f point;
    pDomain* zone;
    bool within;
    float power;
    float epsilon;

public:
    RepelZone(Vec3f &point, pDomain &zone, float power, float epsilon=100, bool within=true);
    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

//Rotates the particles about the sphere's outer radius
class Orbit: public ParticleEffect {
private:
    PDSphere* shell;
    int count;
	float thetaInitial;

public:
    Orbit(PDSphere &shell, float thetaInitial = 0.0);
    ~Orbit();
    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

class KillZone: public ParticleEffect {
private:
    pDomain* domain;
    bool within;
    
public:
    KillZone(pDomain &domain, bool within=true);
    ~KillZone();
    void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

class Repulse: public ParticleEffect {
private:
	Vec3f position;
	float power;
	
public:
	Repulse(Vec3f position, float power);
	~Repulse();
	void execute(ParticleSystem *particleSys, ParticleGroup &particleGroup);
	void execute(Particle &p, float dt);
	ParticleEffect* copy();
};

#endif