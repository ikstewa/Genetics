#include "ParticleSystem.h"

//Initialize with default values
ParticleSystem::ParticleSystem(int numParticles) : particleGroup(numParticles) {	
	position = new PDPoint(P000);
	velocity = new PDPoint(P000);
	color = new PDPoint(P000);
	rot = new PDPoint(P000);
	size = new PDPoint(P111);

	mass = 1.0f;
	massStdDev = 0.0f;
	alpha = 1.0f;
	alphaStdDev = 0.0f;
	age = 0.0f;
	ageStdDev = 0.0f;
	dt = 1.0f;
	emitter = NULL;
}

ParticleSystem::~ParticleSystem() {
	delete velocity;
	delete color;
	delete rot;
	delete size;
	delete position;
	delete emitter;

	mass = 0.0f;
	alpha = 0.0f;
	age = 0.0f;

	//particleGroup.~ParticleGroup();
}

void ParticleSystem::initialize() {
	int i;
	int maxParticles = (int)particleGroup.getMaxParticles();
	Particle *p;

	for (i=0; i<maxParticles; i++) {
		p = new Particle();
        initializeParticle(p);

		particleGroup.add(p);
	}
}

void ParticleSystem::initializeParticle(Particle* p) {
    p->vel = velocity->Generate();
	p->color = color->Generate();
	p->pos = position->Generate();
	p->size = size->Generate();
    p->rot = rot->Generate();
	p->alpha = alpha - alphaStdDev + (2.0f*alphaStdDev)*pRandf();
	p->mass = mass - massStdDev + (2.0f*massStdDev)*pRandf();
	p->age = age - ageStdDev + (2.0f*ageStdDev)*pRandf();
    p->angularVel = angularVel - angularVelStdDev + (2.0f*angularVelStdDev)*pRandf();
}

void ParticleSystem::setEmitter(ParticleEmitter* emitter) {
	this->emitter = emitter;
}

void ParticleSystem::setPosition(const pDomain &domain) {
	delete position;
	position = domain.copy();
}

void ParticleSystem::setColor(const pDomain &domain) {
	delete color;
	color = domain.copy();
}

void ParticleSystem::setVelocity(const pDomain &domain) {
	delete velocity;
	velocity = domain.copy();
}

void ParticleSystem::setRot(const pDomain &domain) { 
	delete rot;
	rot = domain.copy();
}

void ParticleSystem::setSize(const pDomain &domain) {
	delete size;
	size = domain.copy();
}

void ParticleSystem::setAlpha(float alpha, float stdDev) {
	this->alpha = alpha;
	this->alphaStdDev = stdDev;
}

void ParticleSystem::setAge(float age, float stdDev) {
	this->age = age;
	this->ageStdDev = stdDev;
}

void ParticleSystem::setMass(float mass, float stdDev) {
	this->mass = mass;
	this->massStdDev = stdDev;
}

void ParticleSystem::setAngularVel(float angularVel, float stdDev) {
    this->angularVel = angularVel;
    this->angularVelStdDev = stdDev;
}

float ParticleSystem::getAngularVel() {
	return angularVel;
}

void ParticleSystem::setMaxParticles(size_t maxParticles) {
	particleGroup.setMaxParticles(maxParticles);
}


size_t ParticleSystem::getMaxParticles() {
	return particleGroup.getMaxParticles();
}

void ParticleSystem::addEffect(ParticleEffect* effect) {
    effectList.push_back(effect);
}

ParticleList::iterator ParticleSystem::getParticleListBegin() {
    return particleGroup.begin();
}

ParticleList::iterator ParticleSystem::getParticleListEnd() {
    return particleGroup.end();
}


void ParticleSystem::setTimeStep(float dt) {
	this->dt = dt;
}


void ParticleSystem::step() {
    ParticleList::iterator particlesBegin;
    ParticleList::iterator particlesEnd;
    int numEffects = (int)effectList.size();
    
    //Apply the effects
    for (int i=0; i<numEffects; i++) {
        effectList[i]->execute(this, particleGroup);    
    }

	if (emitter != NULL) { //update the emitter if one exists
		emitter->update(dt);
		emitter->emit(this, particleGroup);
	}

    particlesBegin = getParticleListBegin();
    particlesEnd = getParticleListEnd();

    //Update position and age
    int count = 0;
    while (particlesBegin != particlesEnd) {
		
        (*particlesBegin)->age += dt;
		(*particlesBegin)->pos += (*particlesBegin)->vel * dt; 

        particlesBegin++;
        count++;
    }
}

int ParticleSystem::getSize() {
    return (int)particleGroup.size();
}

float ParticleSystem::getDt() {
    return dt;
}   

void ParticleSystem::setTextId(int textId) {
    this->textId = textId;
}

int ParticleSystem::getTextId() {
    return textId;
}

void ParticleSystem::setParticleSysLoc(Vec3f pos) {
	this->pos = pos;
}

Vec3f ParticleSystem::getParticleSysLoc() {
	return pos;
}

void ParticleSystem::setVelScale(Vec3f velScale) {
	this->velScale = velScale;
}

Vec3f ParticleSystem::getVelScale() {
	return velScale;
}

void ParticleSystem::setType(int type) {
	this->type = type;
}

int ParticleSystem::getType() {
	return type;
}