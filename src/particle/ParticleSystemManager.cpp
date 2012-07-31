#include "ParticleSystemManager.h"
#include "../render/TextureLibrary.h"

bool ParticleSystemManager::enableParticles = true;

ParticleSystemManager* ParticleSystemManager::getInstance() {
    //Will be constructed on the first call to getInstance.
    if (enableParticles)
    {
        static ParticleSystemManager pSysMan; 
        return &pSysMan;
    }
    else
        return NULL;
}


size_t ParticleSystemManager::genParticleSystems(int num, int maxSize) {
	size_t startIndex = particleSysList.size();		
	particleSysList.reserve(startIndex+num);
	
	for (size_t i=startIndex; i<startIndex+num; i++) {
		particleSysList.push_back(new ParticleSystem(maxSize));
	}

	return startIndex;
}

size_t ParticleSystemManager::getNumberParticleSystems() {
    return particleSysList.size();
}

void ParticleSystemManager::setActiveParticleSystem(size_t id) {
	if (id >= particleSysList.size()) {
		//Throw an error
		fprintf(stderr, "setActiveParticleSystem: id %d out of range.\n", id);
	}else if (id < 0) {
		fprintf(stderr, "setActiveParticleSystem: negative id.\n");
		//Throw an error
	}

	activeId = (int)id;
	activeParticleSys = particleSysList[id];
}

void ParticleSystemManager::deleteParticleSystem(size_t id) {
	if (id < 0 || id >= particleSysList.size()) {
		//ERROR, invalid id
	}

	//Remove all the particles from the system while maintaining id order.
	particleSysList[id]->setMaxParticles(0);
	particleSysList[id]->~ParticleSystem();

	std::vector<ParticleSystem*>::iterator list = particleSysList.begin();
	list += id;

	if (list != particleSysList.end() - 1) {
		*list = *(particleSysList.end() - 1);
		//particleSysList[id] = particleSysList.end() - 1;
		particleSysList.pop_back();
	}else {
		particleSysList.pop_back();
	}
}

void ParticleSystemManager::setPosition(const Vec3f &vec) {
	setPosition(PDPoint(vec));
}
void ParticleSystemManager::setPosition(const pDomain &domain){
	activeParticleSys->setPosition(domain);
}

void ParticleSystemManager::setColor(const Vec3f &vec) {
	setColor(PDPoint(vec));
}

void ParticleSystemManager::setColor(const pDomain &domain) {
	activeParticleSys->setColor(domain);
}

void ParticleSystemManager::setVelocity(const Vec3f &vec) {
	setVelocity(PDPoint(vec));
}
void ParticleSystemManager::setVelocity(const pDomain &domain) {
	activeParticleSys->setVelocity(domain);
}

void ParticleSystemManager::setRot(const Vec3f &vec) {
	setRot(PDPoint(vec));
}
void ParticleSystemManager::setRot(const pDomain &domain) {
	activeParticleSys->setRot(domain);
}

void ParticleSystemManager::setSize(const Vec3f &vec) {
	setSize(PDPoint(vec));
}

void ParticleSystemManager::setSize(const pDomain &domain) {
	activeParticleSys->setSize(domain);
}

void ParticleSystemManager::setAlpha(float alpha, float stdDev) {
	activeParticleSys->setAlpha(alpha, stdDev);
}

void ParticleSystemManager::setAge(float age, float stdDev) {
	activeParticleSys->setAge(age, stdDev);
}

void ParticleSystemManager::setMass(float mass, float stdDev) {
	activeParticleSys->setMass(mass, stdDev);
}

void ParticleSystemManager::setAngularVel(float angularVel, float stdDev) {
	activeParticleSys->setAngularVel(angularVel, stdDev);
}

float ParticleSystemManager::getAngularVel() {
	return activeParticleSys->getAngularVel();		
}

void ParticleSystemManager::setMaxParticles(size_t maxParticles) {
	activeParticleSys->setMaxParticles(maxParticles);
}

void ParticleSystemManager::setTimeStep(float dt) {
	activeParticleSys->setTimeStep(dt);
}

size_t ParticleSystemManager::getMaxParticles() {
	return activeParticleSys->getMaxParticles();
}

void ParticleSystemManager::initalizeActiveSystem() {
	activeParticleSys->initialize();
}


ParticleList::iterator ParticleSystemManager::getParticleListBegin() {
    return activeParticleSys->getParticleListBegin();
}

ParticleList::iterator ParticleSystemManager::getParticleListEnd() {
    return activeParticleSys->getParticleListEnd();
}


void ParticleSystemManager::step() {
    this->activeParticleSys->step();
}

void ParticleSystemManager::setTextureId(int texId) {    
    this->activeParticleSys->setTextId(texId);
}

int ParticleSystemManager::getTextureId() {
    return this->activeParticleSys->getTextId();
}

void ParticleSystemManager::setParticleSysPos(Vec3f pos) {
	this->activeParticleSys->setParticleSysLoc(pos);
}

Vec3f ParticleSystemManager::getParticleSysPos() {
	return this->activeParticleSys->getParticleSysLoc();
}

void ParticleSystemManager::setVelScale(Vec3f velScale) {
	this->activeParticleSys->setVelScale(velScale);
}

Vec3f ParticleSystemManager::getVelScale() {
	return this->activeParticleSys->getVelScale();
}


void ParticleSystemManager::makeBillboardedModelView(float *modelview) {
    modelview[0] = modelview[5] = modelview[10] = 1.0;

    modelview[1] = modelview[2] = 0.0;
    modelview[4] = modelview[6] = 0.0;
    modelview[8] = modelview[9] = 0.0;
}

void ParticleSystemManager::setupOrbCollect(Vec3f pos, Vec3f playerPos, Vec3f color) {
	size_t id;
	std::vector<pDomain*> domainList;
	
	this->setActiveParticleSystem(3); //center of orb. Magic numbers make me cry.
	ParticleList::iterator particles = this->getParticleListBegin();
	ParticleList::iterator particlesEnd = this->getParticleListEnd();

	while (particles != particlesEnd) {
		domainList.push_back(new PDPoint((*particles)->pos + pos));
		particles++;
	}

	id = this->genParticleSystems(1, 10);
	this->setActiveParticleSystem(id);
	this->setType(ORB_COLLECT_SYSTEM);
	this->setTimeStep(0.04);
	this->setSize(Vec3f(0.2, 0.2, 0.2));
	this->setColor(color);
	this->setPosition(PDUnion(domainList));
	this->setVelocity(PDDisc(Vec3f(0.0, 15.0, 0.0), Vec3f(0, 1.0, 0), 3.0, 3.0));
	this->gravity(Vec3f(0.0, -35.0, 0.0));
	this->killAge(5.0, 0.0);
	this->fade(0.01, 0.003);

	this->initalizeActiveSystem();	
}

void ParticleSystemManager::setupDustCloud(Vec3f position, Vec3f vel) {
	size_t id;
	ParticleEmitter* emitter;
	Particle p;
	Vec3f scaledVel, normalizedVel;
	int numSystems = 25;
	float drag, dragIncrement;
	float xfact, zfact;
	float power = abs(vel.y) - 10.0;
	float divisor;

	if (power >= 40.0) { //50 is the maximum power supported 
		power = 40.0;
	}
	//printf("power: %f vel: %f, %f, %f\n", power, vel.x, vel.y, vel.z);

	//exponential growth formula to determine number of particle systems.
	numSystems = 25 * pow(2, power / 20.0);
	drag = 0.4 - 0.4 * (power / 40.0);
	dragIncrement = 0.01 * (power / 40.0);

	id = this->genParticleSystems(numSystems, 7);
	//id = this->genParticleSystems(1000, 15);
    PDSubtractive domain = PDSubtractive(PDSphere(Vec3f(), 50.0, 20.0),
                                         PDBox(Vec3f(-50.0, -50.0, -50.0), Vec3f(50.0, 0.0, 50.0)),
										 PDBox(Vec3f(-50.0, 10.0, -50.0), Vec3f(50.0, 50.0, 50.0)));

	normalizedVel = vel / vel.length();
	divisor = abs(vel.x) + abs(vel.z);
	xfact = divisor == 0.0 ? 0.0 : vel.x / divisor;  
	zfact = divisor == 0.0 ? 0.0 : vel.z / divisor;
	//xfact = vel.x / (vel.x+vel.z);
	//xfact = xfact < 0.0 ? 0.0 : xfact; 
	//zfact = vel.z / (vel.x+vel.z);
	//zfact = zfact < 0.0 ? 0.0 : zfact; 
	//printf("xfact: %f zfact: %f\n", xfact, zfact);

	/*PDSubtractive domain = PDSubtractive(PDSphere(Vec3f(50.0*xfact, 0.0, 50.0*zfact), 50.0, divisor == 0.0 ? 20.0 : 0.0),
                                         PDBox(Vec3f(-100.0, -100.0, -100.0), Vec3f(100.0, 0.0, 100.0)),
										 PDBox(Vec3f(-100.0, 10.0, -100.0), Vec3f(100.0, 100.0, 100.0)));*/
										 //PDCone(Vec3f(0.0, -10.0, 0.0), Vec3f(0.0, 50.0, 0.0), 45.0));
                                         //PDSphere(PDSphere(Vec3f(0.0, 50.0, 0.0), 50.0)));
		
	/*PDSubtractive domain = PDSubtractive(PDEllipsoid(Vec3f(), 50.0, 50.0, 50.0), 
									     PDEllipsoid(Vec3f(), 20.0, 20.0, 20.0),
									     PDBox(Vec3f(-50.0, -50.0, -50.0), Vec3f(50.0, 0.0, 50.0)),
									     PDBox(Vec3f(-50.0, 10.0, -50.0), Vec3f(50.0, 50.0, 50.0)));*/

	PDSphere rotDomain = PDSphere(Vec3f(), 1.0, 0.1);

	for (int i=0; i<numSystems; i++) {
		this->setActiveParticleSystem(id+i);
		this->setType(DUST_CLOUD_SYSTEM);
		this->setAlpha(0.75, 0.03);
		this->setTimeStep(0.02);
		//this->quadraticDrag(0.0, 0.01);
		this->quadraticDrag(drag, dragIncrement);
		this->linearDrag(0.5);
		this->killAge(1.8, 0.3);
		this->setSize(PDLine(Vec3f(0.6, 0.6, 0.6), Vec3f(1.0, 1.0, 1.0)));
		this->fade(0.016, 0.009);
		this->setColor(Vec3f(1.0, 1.0, 1.0));
		this->setVelocity(domain.Generate() /*+ Vec3f(vel.x*0.5, 0.0, vel.z*0.5)*/);
		//this->setVelScale(scaledVel);
		this->setPosition(PDSphere(position, 1.0));
		this->setAngularVel(1.5, 0.5);
		this->setRot(rotDomain.Generate());
		//this->orbit(PDSphere(Vec3f(), 1.0));
		this->initalizeActiveSystem();
	}

}

void ParticleSystemManager::setupCloud(Vec3f position) {
	size_t id;

	id = this->genParticleSystems(1, 40);

	this->setActiveParticleSystem(id);
	this->setType(CLOUD_SYSTEM);
	this->setTimeStep(0.02);
	this->setSize(PDLine(Vec3f(6.0, 6.0, 6.0), Vec3f(7.5, 7.5, 7.5)));
	this->setPosition(PDEllipsoid(position, 21.0, 12.0, 12.0));
	this->setAlpha(0.4, 0.15);
	this->setVelocity(Vec3f(pRandf() * 2.0, 0.0, 0.0));
	this->killZone(PDBox(Vec3f(-250.0, 200.0, -250.0), Vec3f(250.0, 400.0, 250.0)), false);

	this->initalizeActiveSystem();

	/*PDEllipsoid domain = PDEllipsoid(position + Vec3f(4.0, 0.0, 0.0), 7.0, 4.0, 4.0);
	id = this->genParticleSystems(50, 5);
	for (int i=0; i<50; i++) {
		this->setActiveParticleSystem(id+i);
		this->setType(DUST_CLOUD_SYSTEM);
		this->setAlpha(0.4, 0.15);
		//this->setSize(PDLine(Vec3f(0.6, 0.6, 0.6), Vec3f(1.0, 1.0, 1.0)));
		//this->setColor(Vec3f(1.0, 1.0, 1.0));
		this->setPosition(PDSphere(domain.Generate(), 1.5));
		this->initalizeActiveSystem();
	}*/
}

void ParticleSystemManager::setupDeath(std::vector<Vec3f> verts) {
	size_t id, i, size;
	std::vector<pDomain*> domainList;

	size = verts.size();
	for (i=0; i<size; i += 3) {
		domainList.push_back(new PDPoint(verts[i]));
	}

	id = this->genParticleSystems(1, domainList.size());
	this->setActiveParticleSystem(id);
	this->setSize(Vec3f(0.1, 0.1, 0.1));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->setPosition(PDUnion(domainList));
	this->gravity(Vec3f(0.0, -10.0, 0.0));
	
	size = domainList.size();
	for (i=0; i<size; i += 3) { //memory management
		delete domainList[i];
	}
}

void ParticleSystemManager::setupMessage(Vec3f position, int type, int texId) {
	size_t id;

	id = this->genParticleSystems(1, 1);
	this->setActiveParticleSystem(id);
	this->setType(type);
	this->setTextureId(texId);
	//this->setPosition(position);
	this->setPosition(Vec3f(0.0, 0.0, 0.0));
	this->setVelocity(Vec3f(0.0, 0.7, 0.0));
	this->fade(0.02);
	this->killAge(1.5, 0.0);
	//this->setSize(Vec3f(1.0, 1.0, 0.0));
    this->setSize(Vec3f(0.1, 0.1, 0.1));

	this->initalizeActiveSystem();
}

void ParticleSystemManager::setupParticleSystems() {
    size_t id;
	ParticleEmitter* emitter;
	Particle p;

	/*start 0*/
	id = this->genParticleSystems(1, 30); 
	this->setActiveParticleSystem(id);
	this->setTextureId(TEXTURE_DOT);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.1, 0.1, 0.1));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->quadraticDrag(0.1, 0.0);
	this->fade(0.05, 0.01);
	this->killAge(1.3, 0.3);

	p.vel.zero();
	p.pos.zero();
	p.angularVel = 1.5;
	p.rot = Vec3f(0.6, 0.0, 0.0);
	emitter = new ParticleEmitter(PDPoint(Vec3f()), 1, p);
	emitter->addEffect(&Orbit(PDSphere(Vec3f(), 1.2)));
	this->emitter(emitter);
	/*end 0*/

	/*start 1*/
	id = this->genParticleSystems(1, 30); 
	this->setActiveParticleSystem(id);
	this->setTextureId(TEXTURE_DOT);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.1, 0.1, 0.1));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->quadraticDrag(0.1, 0.0);
	this->fade(0.05, 0.01);
	this->killAge(1.3, 0.3);

	p.vel.zero();
	p.pos.zero();
	p.angularVel = 1.5;
	p.rot = Vec3f(-0.6, 0.0, 0.0);
	emitter = new ParticleEmitter(PDPoint(Vec3f()), 1, p);
	emitter->addEffect(&Orbit(PDSphere(Vec3f(), 1.2)));
	this->emitter(emitter);
	/*end 1*/

	/*start 2*/
	id = this->genParticleSystems(1, 30); 
	this->setActiveParticleSystem(id);
	this->setTextureId(TEXTURE_DOT);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.1, 0.1, 0.1));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->quadraticDrag(0.1, 0.0);
	this->fade(0.05, 0.01);
	this->killAge(1.3, 0.3);

	p.vel.zero();
	p.pos.zero();
	p.angularVel = 1.5;
	p.rot = Vec3f(0.0, 1.0, 0.0);
	emitter = new ParticleEmitter(PDPoint(Vec3f()), 1, p);
	emitter->addEffect(&Orbit(PDSphere(Vec3f(), 1.2)));
	this->emitter(emitter);
	/*end 2*/

	/*start 3*/
	id = this->genParticleSystems(1, 5); 
	this->setActiveParticleSystem(id);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.2, 0.2, 0.2));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->setAngularVel(0.4, 0.2);

	std::vector<pDomain*> domainList;
	//need memory management here, point gets copied, memory leak....
    domainList.push_back(new PDPoint(Vec3f(0.6, 0.0, 0.4)));
    domainList.push_back(new PDPoint(Vec3f(-1.0, 0.0, 0.0)));    
    domainList.push_back(new PDPoint(Vec3f(0.7, 1.0, 0.0)));
	domainList.push_back(new PDPoint(Vec3f(0.0, -1.0, 0.0)));

	this->setRot(PDUnion(domainList));
    this->orbit(PDSphere(Vec3f(0.0, 0.0, 0.0), 0.20));
    this->initalizeActiveSystem();
	/*end 3*/

	/*start 4*/
	id = this->genParticleSystems(1, 15); 
	this->setActiveParticleSystem(id);
	//this->setTimeStep(0.06);
	this->setSize(PDLine(Vec3f(0.6, 0.6, 0.6), Vec3f(1.0, 1.0, 1.0)));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	//this->setAngularVel(0.4, 0.2);

	//std::vector<pDomain*> domainList;
	domainList.clear();
	//MEMORY MANAGEMENT!!!!
    domainList.push_back(new PDSphere(Vec3f(0.0, 7.0, 0.0), 1.0));

	//this->setPosition(PDSubtractive(PDUnion(domainList), PDBox(Vec3f(-5.0, -5.0, -5.0), Vec3f(5.0, 0.0, 5.0))));
	this->setPosition(PDSubtractive(PDUnion(domainList)));

	this->initalizeActiveSystem();
	/*end 4*/



	/*start 5*/
	id = this->genParticleSystems(1, 30); 
	this->setActiveParticleSystem(id);
	this->setTextureId(TEXTURE_DOT);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.5, 0.5, 0.5));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->quadraticDrag(0.1, 0.0);
	this->fade(0.05, 0.01);
	this->killAge(1.3, 0.3);

	p.vel.zero();
	p.pos.zero();
	p.angularVel = 1.5;
	p.rot = Vec3f(0.6, 0.0, 0.0);
	emitter = new ParticleEmitter(PDPoint(Vec3f()), 1, p);
	emitter->addEffect(&Orbit(PDSphere(Vec3f(), 6.0)));
	this->emitter(emitter);
	/*end 5*/

	/*start 6*/
	id = this->genParticleSystems(1, 30); 
	this->setActiveParticleSystem(id);
	this->setTextureId(TEXTURE_DOT);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.5, 0.5, 0.5));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->quadraticDrag(0.1, 0.0);
	this->fade(0.05, 0.01);
	this->killAge(1.3, 0.3);

	p.vel.zero();
	p.pos.zero();
	p.angularVel = 1.5;
	p.rot = Vec3f(-0.6, 0.0, 0.0);
	emitter = new ParticleEmitter(PDPoint(Vec3f()), 1, p);
	emitter->addEffect(&Orbit(PDSphere(Vec3f(), 6.0)));
	this->emitter(emitter);
	/*end 6*/

	/*start 7*/
	id = this->genParticleSystems(1, 30); 
	this->setActiveParticleSystem(id);
	this->setTextureId(TEXTURE_DOT);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.5, 0.5, 0.5));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->quadraticDrag(0.1, 0.0);
	this->fade(0.05, 0.01);
	this->killAge(1.3, 0.3);

	p.vel.zero();
	p.pos.zero();
	p.angularVel = 1.5;
	p.rot = Vec3f(0.0, 1.0, 0.0);
	emitter = new ParticleEmitter(PDPoint(Vec3f()), 1, p);
	emitter->addEffect(&Orbit(PDSphere(Vec3f(), 6.0)));
	this->emitter(emitter);
	/*end 7*/

	/*start 8*/
	id = this->genParticleSystems(1, 5); 
	this->setActiveParticleSystem(id);
	this->setTimeStep(0.06);
	this->setSize(Vec3f(0.2, 0.2, 0.2));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	this->setAngularVel(0.4, 0.2);

	domainList.clear();
	//need memory management here, point gets copied, memory leak....
    domainList.push_back(new PDPoint(Vec3f(3.0, 0.0, 2.0)));
    domainList.push_back(new PDPoint(Vec3f(-5.0, 0.0, 0.0)));    
    domainList.push_back(new PDPoint(Vec3f(3.5, 5.0, 0.0)));
	domainList.push_back(new PDPoint(Vec3f(0.0, -5.0, 0.0)));

	this->setRot(PDUnion(domainList));
    this->orbit(PDSphere(Vec3f(0.0, 0.0, 0.0), 1.0));
    this->initalizeActiveSystem();
	/*end 8*/

	/*start 9*/
	id = this->genParticleSystems(1, 15); 
	this->setActiveParticleSystem(id);
	//this->setTimeStep(0.06);
	this->setSize(PDLine(Vec3f(0.6, 0.6, 0.6), Vec3f(1.0, 1.0, 1.0)));
	this->setColor(Vec3f(1.0, 1.0, 1.0));
	//this->setAngularVel(0.4, 0.2);

	//std::vector<pDomain*> domainList;
	domainList.clear();
	//MEMORY MANAGEMENT!!!!
    domainList.push_back(new PDSphere(Vec3f(0.0, 7.0, 0.0), 1.0));
	//domainList.push_back(new PDSphere(Vec3f(1.8, 6.8, 0.0), 0.5));
	//domainList.push_back(new PDSphere(Vec3f(4.0, 6.5, 0.0), 1.0));
	//domainList.push_back(new PDSphere(Vec3f(3.5, 0.0, 0.0), 3.0));
	//domainList.push_back(new PDSphere(Vec3f(0, 0.0, 3.5), 3.0));
	//domainList.push_back(new PDSphere(Vec3f(0, 0.0, -3.5), 3.0));
	//domainList.push_back(new PDSphere(Vec3f(0, 0.0, -3.5), 3.0));
	//domainList.push_back(new PDSphere(Vec3f(-1.0, 3.0, 1.0), 3.0));
	//domainList.push_back(new PDSphere(Vec3f(1.0, 3.0, -1.0), 3.0));

	//this->setPosition(PDSubtractive(PDUnion(domainList), PDBox(Vec3f(-5.0, -5.0, -5.0), Vec3f(5.0, 0.0, 5.0))));
	this->setPosition(PDSubtractive(PDUnion(domainList)));

	this->initalizeActiveSystem();
	/*end 9*/

	PDBox box = PDBox(Vec3f(-200.0, 250.0, -200), Vec3f(200.0, 275, 200.0));

	for (int i=0; i<10; i++) {
		this->setupCloud(box.Generate());
	}
}

//Particle Effects
/*
void ParticleSystemManager::emitter(const pDomain &domain, int rate) {
    Emitter* emitter = new Emitter(domain, rate);
    this->activeParticleSys->addEffect(emitter);
}*/

void ParticleSystemManager::friction(float friction) {
    Friction* frictionEffect = new Friction(friction);
    this->activeParticleSys->addEffect(frictionEffect);
}

void ParticleSystemManager::quadraticDrag(float drag, float increment) {
    QuadraticDrag* quadDrag = new QuadraticDrag(drag, increment);
    this->activeParticleSys->addEffect(quadDrag);
}

void ParticleSystemManager::linearDrag(float drag) {
    LinearDrag* linearDrag = new LinearDrag(drag);
    this->activeParticleSys->addEffect(linearDrag);
}

void ParticleSystemManager::killAge(float maxAge, float sigma) {
    KillAge* killAge = new KillAge(maxAge, sigma);
    this->activeParticleSys->addEffect(killAge);
}

void ParticleSystemManager::fade(float fadeRate, float sigma) {
    Fade* fade = new Fade(fadeRate,sigma);
    this->activeParticleSys->addEffect(fade);
}

void ParticleSystemManager::accelerateZone(pDomain &dirDomain, pDomain &zone, bool within) {
    AccelerateZone* accelZone = new AccelerateZone(dirDomain, zone, within);
    this->activeParticleSys->addEffect(accelZone);
}

void ParticleSystemManager::attractZone(Vec3f point, pDomain &zone, float power, float epsilon, bool within) {
    AttractZone* attractZone = new AttractZone(point, zone, power, epsilon, within);
    this->activeParticleSys->addEffect(attractZone);
}


void ParticleSystemManager::orbit(PDSphere &shell) {
    Orbit* orbit = new Orbit(shell);
    this->activeParticleSys->addEffect(orbit);
}

void ParticleSystemManager::killZone(pDomain &domain, bool within) {
    KillZone* killZone = new KillZone(domain, within);
    this->activeParticleSys->addEffect(killZone);
}

void ParticleSystemManager::repulse(Vec3f position, float power) {
	Repulse* repulse = new Repulse(position, power);
	this->activeParticleSys->addEffect(repulse);
}

void ParticleSystemManager::gravity(Vec3f dir) {
	Gravity* gravity = new Gravity(dir);
	this->activeParticleSys->addEffect(gravity);
}

void ParticleSystemManager::emitter(ParticleEmitter* emitter) {
	this->activeParticleSys->setEmitter(emitter);
}

int ParticleSystemManager::getType() {
	return this->activeParticleSys->getType();
}

void ParticleSystemManager::setType(int type) {
	this->activeParticleSys->setType(type);
}
