#include "Cloud.h"

Cloud::Cloud(const PDEllipsoid &domain) {
    this->domain = (PDEllipsoid*)domain.copy();
    //this->cellHeight = cellHeight;
    //this->cellLength = cellLength;
    //this->cellWidth = cellWidth;
    this->firstExecute = true;
}

Cloud::~Cloud() {
    delete domain;
    cellAutomaton.clear();
}

void Cloud::execute(ParticleGroup & particleGroup) {    
    ParticleList::iterator begin = particleGroup.begin();
    ParticleList::iterator end = particleGroup.end();
    int count = 0;
    int test = 0;
    Cell32* cell32;

    if (firstExecute) {
        firstExecute = false;
        constructCellularAutomaton(particleGroup);
    }

    performFormationAndExtinction(particleGroup);
    performTransition(particleGroup); //TODO: performTransition is not a complete implementation

    while (begin != end) {
        if (count % NUM_CELLS_PER_CELL32 == 0) {
            cell32 = cellAutomaton[count / NUM_CELLS_PER_CELL32];
        }

        if((cell32->cloud & (0x01 << (count%NUM_CELLS_PER_CELL32))) != 0) {
            (*begin)->age = 0.0;
            
        }else {
            test++;
            (*begin)->age = 1.0;
        }

        begin++;
        count++;
    }
}

void Cloud::performFormationAndExtinction(ParticleGroup & particleGroup) {
    int numCells = (int)particleGroup.getMaxParticles();
    int cellsForSet;

    ParticleList::iterator begin = particleGroup.begin();
    int numAutomaton = (int)cellAutomaton.size();
    int i,j;
    Cell32* cell;
    float rand;

    for (i=0; i<numAutomaton; i++) {
        cell = cellAutomaton[i];
        cellsForSet = ((i+1)*NUM_CELLS_PER_CELL32 >= numCells) ? numCells%NUM_CELLS_PER_CELL32 : NUM_CELLS_PER_CELL32;
        for (j=0; j<cellsForSet; j++) { 
            rand = pRandf();
            if ((cell->hum & (0x01 << j)) != 0) { //if the cell is humid
                if (cell->pAct[j] > rand) {
                    cell->act |= 0x01 << j; 
                }
            }else if ((cell->cloud & (0x01 << j)) != 0) { //if the cell is a cloud
                if (cell->pExt[j] > rand) {
                    cell->cloud ^= 0x01 << j;

                    //give the cell a probability to become humid
                    //the probability of becoming humid is directly related to the distance from the center
                    cell->pExt[j] = 0.0;
                    cell->pHum[j] = 1.0f - pRandf() * domain->normDistanceToCenter((*begin)[i*NUM_CELLS_PER_CELL32+j].pos);
                }
            }else { //if empty cell
                if (cell->pHum[j] > rand) {
                    cell->hum |= 0x01 << j; 

                    //give the cell a probability to become active
                    //the probability of becoming active is directly related to the distance from the center
                    cell->pHum[j] = 0.0;
                    cell->pAct[j] = 1.0f - pRandf() * domain->normDistanceToCenter((*begin)[i*NUM_CELLS_PER_CELL32+j].pos);
                }
            }
        }
    }
}

void Cloud::performTransition(ParticleGroup & particleGroup) {
    ParticleList::iterator begin = particleGroup.begin();
    int numAutomaton = (int)cellAutomaton.size();
    Cell32* cell32;
    int i, count;
    int cloud32;
    
    for (i=0; i<numAutomaton; i++) {
        cell32 = cellAutomaton[i];

        cloud32 = cell32->act & cell32->hum;
        count = 0;
        while (cloud32 != 0 && count < NUM_CELLS_PER_CELL32) {
            if ((cloud32 & 0x01) == 1) { //this cell becomes a cloud
                cell32->cloud |= (0x01 << count);

                cell32->hum ^= 0x01 << count; //no longer humid
                cell32->act ^= 0x01 << count; //no longer active
                cell32->pAct[count] = 0.0;
                cell32->pExt[count] = 
                        1.0f - pRandf() * domain->normDistanceToCenter((*begin)[i*NUM_CELLS_PER_CELL32+count].pos);
            }
            cloud32 >>= 1;
            count++;
        }
    }

    //TODO: Cells need to become active based on the activity of their neighboring cells...
}

void Cloud::constructCellularAutomaton(ParticleGroup & particleGroup) {
    ParticleList::iterator begin = particleGroup.begin();
    ParticleList::iterator end = particleGroup.end();
    size_t maxParticles = particleGroup.getMaxParticles();
    size_t numCell32 = maxParticles / 32 + (maxParticles%32 == 0 ? 0 : 1);
    Cell32* cell;
    int count = 0;
    float normDist = 0;

    cellAutomaton.reserve(numCell32);

    while (begin != end) {   
        if (count % 32 == 0) {
            cell = new Cell32();
            cellAutomaton.push_back(cell);

            cell->act = 0;
            cell->cloud = 0;
            cell->hum = 0;
        }   

        //All cells start out empty with a probability to become humid.
        //The probability is directly related to its distance from the center of the ellipsoid.
        normDist = domain->normDistanceToCenter((*begin)->pos);
        cell->pHum[count % 32] = 1.0f - pRandf() * normDist;
        cell->pAct[count % 32] = 0.0;
        cell->pExt[count % 32] = 0.0;

        begin++;
        count++;
    }
}