#ifndef	__PARTICLE_SYSTEM_MANAGER_H
#define __PARTICLE_SYSTEM_MANAGER_H
#include "ParticleSystem.h"
#include "ParticleEffect.h"

#define DUST_CLOUD_SYSTEM 0
#define ORB_COLLECT_SYSTEM 1
#define CLOUD_SYSTEM 2
#define MESSAGE_SYSTEM 3

struct pPosTypeTuple {
	int type;
	Vec3f pos;
	Vec3f color;
};

class ParticleSystemManager {

private:
	int activeId;
	ParticleSystem *activeParticleSys;
	std::vector<ParticleSystem*> particleSysList;
	static const int COORDS_PER_POSITION_VERTEX = 3;
    int textId;

protected:
	ParticleSystemManager() {
		activeId = -1; //No systems have been generated yet.

	}

	~ParticleSystemManager() {

	}

public:
    static ParticleSystemManager* getInstance();
    static bool enableParticles;

	//Generates [num] particle systems with a maxium of [maxSize] particles in each system.
	//The return value is the ID of the first particle system generated. 
	//The ID numbers are guaranteed to be consecutive.
	size_t genParticleSystems(int num, int maxSize);

    size_t getNumberParticleSystems();

	void setActiveParticleSystem(size_t id);

	void deleteParticleSystem(size_t id);

    //TODO: implement
    //void emitter(const pDomain &domain, int rate);
    void friction(float friction);
    void quadraticDrag(float drag, float increment);
    void linearDrag(float drag);
    void killAge(float maxAge, float sigma);
    void fade(float fadeRate, float sigma=0);
    void accelerateZone(pDomain &dirDomain, pDomain &zone, bool within=true);
    void attractZone(Vec3f point, pDomain &zone, float power, float epsilon=100, bool within=true);
    void orbit(PDSphere &shell);
    void killZone(pDomain &domain, bool within = true);
	void repulse(Vec3f position, float power);
	void gravity(Vec3f dir);
	//TODO: Shouldn't send a pointer, 50% hack
	void emitter(ParticleEmitter* emitter);
    //done


	void setupDustCloud(Vec3f position, Vec3f velocity);
	void setupOrbCollect(Vec3f pos, Vec3f playerPos, Vec3f color);
	void setupCloud(Vec3f position);
	void setupMessage(Vec3f position, int type, int texId);
	void setupDeath(std::vector<Vec3f> verts);

	//Functions for specifying the state of the active particle system.
	void setPosition(const Vec3f &vec); 
	void setPosition(const pDomain &domain);

	void setColor(const Vec3f &vec);
	void setColor(const pDomain &domain);

	void setVelocity(const Vec3f &vec);
	void setVelocity(const pDomain &domain);

	void setRot(const Vec3f &vec);
	void setRot(const pDomain &domain);

	void setSize(const Vec3f &vec);
	void setSize(const pDomain &domain);

	void setAlpha(float alpha, float stdDev);

	void setAge(float age, float stdDev);

	void setMass(float mass, float stdDev);

    void setAngularVel(float angularVel, float stdDev);
	float getAngularVel();

	void setParticleSysPos(Vec3f pos);
	Vec3f getParticleSysPos();
	void setVelScale(Vec3f pos);
	Vec3f getVelScale();

	void setMaxParticles(size_t maxParticles);

	void setTimeStep(float dt);

	size_t getMaxParticles();

	//***IMPORTANT: Call initializeActiveSystem when you are finished specifying the state values.
	//The function applies the state values to each particle of the system.
	void initalizeActiveSystem();

	//void getParticlePositionPointer(int *size, int* stride, float* &ptr); 


    ParticleList::iterator ParticleSystemManager::getParticleListBegin();
    ParticleList::iterator ParticleSystemManager::getParticleListEnd();

    //Temporary function for interacting with world, called by GameEngine to initialize test particle systems
    void setupParticleSystems();

    void step();

    //TODO: Need a better implementation for storing texture ids in the manager.
    void setTextureId(int texId);
    int getTextureId();

    void makeBillboardedModelView(float *modelview);

    void addCloudEffect(const PDEllipsoid &domain);

	int getType();
	void setType(int type);
};

#endif
