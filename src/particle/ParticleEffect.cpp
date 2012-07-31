#include "ParticleEffect.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "../util/Matrix.h"

Gravity::Gravity(Vec3f dir) {
	this->dir = dir;
}

void Gravity::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float dt = particleSys->getDt();

    while (particles != particlesEnd) {
        (*particles)->vel += dir * dt;
        particles++;
    }
}

void Gravity::execute(Particle &p, float dt)  {
	p.vel += dir * dt;
}

ParticleEffect* Gravity::copy() {
	Gravity* gravity = new Gravity(this->dir);
	return gravity;
}
//END gravity

Fade::Fade(float fadeRate, float sigma) {
	this->fadeRate = fadeRate;
	this->sigma = sigma;
}

void Fade::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();

    while (particles != particlesEnd) {
        (*particles)->alpha -= fadeRate + (-sigma + 2.0f*pRandf()*sigma);

        particles++;
    }
}

void Fade::execute(Particle &p, float dt)  {} //Not an applicable effect for an p

ParticleEffect* Fade::copy() {
	Fade* fade = new Fade(this->fadeRate, this->sigma);
	return fade;
}
//End fade

//p
/*
p::p(const pDomain &position, int rate) {
    this->position = position.copy();
    this->rate = rate;
}

void p::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {  
    int maxParticles = (int)particleGroup.getMaxParticles();
    int size = (int)particleGroup.size();
    int numToGenerate;
    int i;
    Particle *p;

    if (size >= maxParticles) { //Already reached the max number of particles
        return;
    }

    numToGenerate = size + rate > maxParticles ? maxParticles - size : rate; 
    //printf("Inside p execute, numToGenerate: %d\n", numToGenerate);

    for (i=0; i<numToGenerate; i++) {
        p = new Particle();
        particleSys->initializeParticle(p);

        p->pos = position->Generate(); //Particle starts at the p position.
        particleGroup.add(*p);
    }
}

ParticleEffect* p::copy() {
    p *e = new p(*this);
    return e;
}*/


//End p

//Kill Age -- Specifies the maxAge a particle lives. Once it reaches that age, it is removed from the system
KillAge::KillAge(float maxAge, float sigma) {
    this->maxAge = maxAge;
    this->sigma = sigma;
}

void KillAge::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particlesBegin = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    
    //kill the particle if it has exceeded maxAge
    while (particlesBegin != particlesEnd) {
        if ((*particlesBegin)->age >= maxAge - sigma + 2.0*pRandf()*sigma) {
            particlesBegin = particleGroup.remove(particlesBegin);
            particlesEnd = particleGroup.end();
            if (particlesBegin == particlesEnd) break;
        }

        particlesBegin++;
    }
}

void KillAge::execute(Particle &p, float dt)  {} //Not an applicable effect for a particle

ParticleEffect* KillAge::copy() {
	KillAge* killAge = new KillAge(this->maxAge, this->sigma);
	return killAge;
}
//End KillAge


//LinearDrag -- Apply a drag force that is proportional to the speed of the particle
LinearDrag::LinearDrag(float drag) {
    this->drag = drag;
}

void LinearDrag::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float scale;
    float dt = particleSys->getDt();

    while (particles != particlesEnd) {
        scale = 1 - (drag * dt / (*particles)->mass);
        if (scale < 0) { //Drag can't change the direction of the particle
            (*particles)->vel.zero();
        }else {
            (*particles)->vel *= scale;
        }
            
        particles++;
    }
}

void LinearDrag::execute(Particle &p, float dt)  {
	float scale = 1 - (drag * dt / p.mass);

    if (scale < 0) { //Drag can't change the direction of the particle
		p.vel.zero();
    }else {		
        p.vel *= scale;
    }
}

ParticleEffect* LinearDrag::copy() {
	LinearDrag* drag = new LinearDrag(this->drag);
	return drag;
}
//End LinearDrag

//QuadraticDrag -- Apply a drag foce that is proportinal to the square of the speed of the particle.
QuadraticDrag::QuadraticDrag(float drag, float increment) {
    this->drag = drag;
	this->increment = increment;
}

void QuadraticDrag::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float scale;
	float dt = particleSys->getDt();
	int i = 0;
	Vec3f dragForce;
	Vec3f normalizedVel;
	float area;

    while (particles != particlesEnd) {
        if ((*particles)->vel.squareLength() == 0) { //Particle isn't moving
            particles++;
            continue;
        }

		//Fd = -(1/2) * p * v^2 * A * Cd * |v|
		normalizedVel = (*particles)->vel / (*particles)->vel.length();
		area = PI * 1.2f * 1.2f;
		dragForce = normalizedVel * (-0.5f * 1.225f * (*particles)->vel.length() * (*particles)->vel.length() * area * drag * dt); 
		//if (i ==0) printf("dragForce: %f, %f, %f\n", dragForce.x, dragForce.y, dragForce.z);

        //scale = 1 - (drag * dt * ((*particles)->vel.length() / (*particles)->mass));
		scale = 1 - (drag * dt * ((*particles)->vel.length() / (*particles)->mass));
		//if (i == 0) printf("scale: %f vel: %f, %f, %f length: %f\n", scale, (*particles)->vel.x, (*particles)->vel.y, (*particles)->vel.z, (*particles)->vel.length());
		/*if (scale < 0) {
			printf("scale: %f vel: %f, %f, %f length: %f\n", scale, (*particles)->vel.x, (*particles)->vel.y, (*particles)->vel.z, (*particles)->vel.length());

		}*/

		//if (i==0) printf("quadDrag: %f, %f, %f scale: %f\n", quadDrag.x, quadDrag.y, quadDrag.z, scale);
		//scale = abs(scale);
		//printf("%d:\tvel: %f, %f, %f\tscale: %f dt: %f mass: %f\n", i, (*particles)->vel.x, particles->vel.y, (*particles)->vel.z, scale, dt, mass);
        //if (scale < 0) { //zero out the velocity, drag can't reverse the direction of a particle
		//	(*particles)->vel.zero();  
        //}else {
			//(*particles)->vel *= scale;
			(*particles)->vel += dragForce;
        //}

        particles++;
		i++;
    }

	drag += increment;
}

void QuadraticDrag::execute(Particle &p, float dt)  {
	float scale;
	if (p.vel.squareLength() == 0) { //p isn't moving
        return;
    }

    scale = 1 - (drag * dt * (p.vel.length() / p.mass));
    if (scale < 0) { //zero out the velocity, drag can't reverse the direction of a particle
        p.vel.zero();  
    }else {
        p.vel *= scale;
    }
}

ParticleEffect* QuadraticDrag::copy() {
	QuadraticDrag* drag = new QuadraticDrag(this->drag, this->increment);
	return drag;
}
//End QuadraticDrag

//Friction -- Apply a constant friction force to slow the particles down.
//The force is constant regardless of the speed of the particle
Friction::Friction(float friction) {
    this->friction = friction;
}

void Friction::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float scale;
    float dt = particleSys->getDt();

    while (particles != particlesEnd) {
        if ((*particles)->vel.squareLength() == 0) { //particle is not moving
            particles++;
            continue;
        }

        scale = 1 - (friction * dt) / ((*particles)->vel.length() * (*particles)->mass);
        if (scale < 0) { //Friction cannot change the direction of the particle
            (*particles)->vel.zero();
        }else {
            (*particles)->vel *= scale;
        }

        particles++;
    }
}

void Friction::execute(Particle &p, float dt)  {
	float scale;
	if (p.vel.squareLength() == 0) { //particle is not moving
        return;
    }

    scale = 1 - (friction * dt) / (p.vel.length() * p.mass);
    if (scale < 0) { //Friction cannot change the direction of the particle
		p.vel.zero();
    }else {
        p.vel *= scale;
    }
}

ParticleEffect* Friction::copy() {
	Friction* friction = new Friction(this->friction);
	return friction;
}
//End Friction


//AccelerateZone
AccelerateZone::AccelerateZone(pDomain &dirDomain, pDomain &zone, bool within) {
    this->dirDomain = dirDomain.copy();
    this->zone = zone.copy();
    this->within = within;
}

void AccelerateZone::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float dt = particleSys->getDt();

    while (particles != particlesEnd) {
        if (zone->Within((*particles)->pos) && within) {
            (*particles)->vel += dirDomain->Generate() * dt;
        }else if (!zone->Within((*particles)->pos) && !within) {
            (*particles)->vel += dirDomain->Generate() * dt;
        }

        particles++;
    }
}

void AccelerateZone::execute(Particle &p, float dt)  {
	if (zone->Within(p.pos) && within) {
        p.vel += dirDomain->Generate() * dt;
    }else if (!zone->Within(p.pos) && !within) {
        p.vel += dirDomain->Generate() * dt;
    }
}

ParticleEffect* AccelerateZone::copy() {
	AccelerateZone* accelZone = new AccelerateZone(*this->dirDomain, *this->zone, this->within);
	return accelZone;
}
//End AccelerateZone


//AttractZone
AttractZone::AttractZone(Vec3f &point, pDomain &zone, float power, float epsilon, bool within) {
    this->zone = zone.copy();
    this->within = within;
    this->power = power;
    this->epsilon = epsilon;
    this->point = point;
}

void AttractZone::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float dt = particleSys->getDt();

    while (particles != particlesEnd) {
        if ((zone->Within((*particles)->pos) && !within) || (!zone->Within((*particles)->pos) && within)) {
            particles++;
            continue; //particle is uneffected by the attractive force
        }

        Vec3f offset = point - (*particles)->pos;
        float length = offset.length();
        float squareLength = offset.squareLength();
        float epsilonSquare = epsilon*epsilon;

        if (squareLength == 0) { //the particle is at the point already
            particles++;
            continue;
        }

        //if( squareLength < epsilonSquare ) squareLength = epsilonSquare;
        float factor = (power * dt) / (squareLength * length);
        (*particles)->vel += offset * factor;

        particles++;
    }
}

void AttractZone::execute(Particle &p, float dt)  {
	if ((zone->Within(p.pos) && !within) || (!zone->Within(p.pos) && within)) {
        return;
    }

    Vec3f offset = point - p.pos;
    float length = offset.length();
    float squareLength = offset.squareLength();
    float epsilonSquare = epsilon*epsilon;

    if (squareLength == 0) { //the p is at the point already
        return;
    }

    //if( squareLength < epsilonSquare ) squareLength = epsilonSquare;
    float factor = (power * dt) / (squareLength * length);
    p.vel += offset * factor;
}

ParticleEffect* AttractZone::copy() {
	AttractZone* attractZone = new AttractZone(this->point, *this->zone, this->power, this->epsilon, this->within);
	return attractZone;
}
//End AttractZone

//RepelZone
RepelZone::RepelZone(Vec3f &point, pDomain &zone, float power, float epsilon, bool within) {
    this->point = point;
    this->zone = zone.copy();
    this->power = power;
    this->epsilon = epsilon;
    this->within = within;
}

void RepelZone::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float dt = particleSys->getDt();

    while (particles != particlesEnd) {
        if ((zone->Within((*particles)->pos) && !within) || (!zone->Within((*particles)->pos) && within)) {
            particles++;
            continue; //particle is uneffected by the repulsive force
        }

        //Vec3f offset = point - (*particles)->pos;
        Vec3f offset = (*particles)->pos - point;
        float length = offset.length();
        float squareLength = offset.squareLength();
        float epsilonSquare = epsilon*epsilon;

        if (squareLength == 0) { //the particle is at the point already
            particles++;
            continue;
        }

        //if( squareLength < epsilonSquare ) squareLength = epsilonSquare;
        float factor = (power * dt) / (squareLength * length);
        (*particles)->vel += offset * factor;

        particles++;
    }
}

void RepelZone::execute(Particle &p, float dt)  {
}

ParticleEffect* RepelZone::copy() {
	RepelZone* repelZone = new RepelZone(this->point, *this->zone, this->power, this->epsilon, this->within);
	return repelZone;
}
//End RepelZone

//Orbit
Orbit::Orbit(PDSphere &shell, float thetaInitial) {
    this->shell = (PDSphere*)shell.copy();
	this->thetaInitial = thetaInitial;
    count = 0;
}

Orbit::~Orbit() {
    delete(shell);
}

void Orbit::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    float dt = particleSys->getDt();
    float theta;
    Matrix mRotX, mRotY, mRotZ;
    count++;

    while (particles != particlesEnd) {
        mRotX.RotX((*particles)->rot.x);
        mRotY.RotY((*particles)->rot.y);
        mRotZ.RotZ((*particles)->rot.z);
        
        theta = dt * count * (*particles)->angularVel;
        (*particles)->pos.x = shell->radOut*cos(theta);
        (*particles)->pos.z = shell->radOut*sin(theta);
        (*particles)->pos.y = 0.0;

        (*particles)->pos = mRotX * (*particles)->pos;
        (*particles)->pos = mRotY * (*particles)->pos;
        (*particles)->pos = mRotZ * (*particles)->pos;

        (*particles)->pos += shell->ctr;
        particles++;
    }
}

void Orbit::execute(Particle &p, float dt)  {
	Matrix mRotX, mRotY, mRotZ;
	float theta;
	count++;
	mRotX.RotX(p.rot.x);
    mRotY.RotY(p.rot.y);
    mRotZ.RotZ(p.rot.z);
    
    theta = dt * count * p.angularVel + thetaInitial;
    p.pos.x = shell->radOut*cos(theta);
    p.pos.z = shell->radOut*sin(theta);
    p.pos.y = 0.0;

    p.pos = mRotX * p.pos;
    p.pos = mRotY * p.pos;
    p.pos = mRotZ * p.pos;

    p.pos += shell->ctr;
}

ParticleEffect* Orbit::copy() {
	Orbit* orbit = new Orbit(*this->shell);
	return orbit;
}
//End Orbit


//Start KillZone
KillZone::KillZone(pDomain &domain, bool within) {
    this->domain = domain.copy();
    this->within = within;
}

KillZone::~KillZone() {
    delete domain;
}

void KillZone::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();

    while (particles != particlesEnd) {
        if ((domain->Within((*particles)->pos) && within) || (!domain->Within((*particles)->pos) && !within)) {
            particles = particleGroup.remove(particles);
            particlesEnd = particleGroup.end();
            if (particles == particlesEnd) break;
        }

        particles++;
    }
}

void KillZone::execute(Particle &p, float dt)  {} //Not applicable

ParticleEffect* KillZone::copy() {
	KillZone* killZone = new KillZone(*this->domain, this->within);
	return killZone;
}
//End KillZone


//Repulse
Repulse::Repulse(Vec3f position, float power) {
	this->position = position;
	this->power = power;
}

Repulse::~Repulse() {
}

void Repulse::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
	ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
	float dt = particleSys->getDt();

    while (particles != particlesEnd) {
		Vec3f offset = (*particles)->pos - position;
		float length = offset.length();
        float squareLength = offset.squareLength();

		float factor = (power * dt) / (squareLength * length);
        (*particles)->vel += offset * factor;

		particles++;
	}
}

void Repulse::execute(Particle &p, float dt) {
}

ParticleEffect* Repulse::copy() {
	Repulse* repulse = new Repulse(this->position, this->power);
	return repulse;
}
//End Repulse