#include "ParticleEffect.h"

//Cell32 represents 32 voxels/cells/particles in the cellular automaton of the cloud.
struct Cell32 {
    unsigned long hum;
    unsigned long act;
    unsigned long cloud;
    float pExt[32];
    float pHum[32];
    float pAct[32];
};

class Cloud: public ParticleEffect {
private:
	PDEllipsoid *domain;
    float cellLength;
    float cellWidth;
    float cellHeight;
    bool firstExecute;
    std::vector<Cell32*> cellAutomaton;
    static const int NUM_CELLS_PER_CELL32 = 32;
    void constructCellularAutomaton(ParticleGroup & particleGroup);
    void performFormationAndExtinction(ParticleGroup & particleGroup);
    void performTransition(ParticleGroup & particleGroup);

public:
	Cloud(const PDEllipsoid &domain);
    ~Cloud();

	void execute(ParticleGroup &particleGroup);
};