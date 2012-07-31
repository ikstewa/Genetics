#ifndef	__BILLBOARD_H
#define __BILLBOARD_H

#include "ParticleGroup.h"
#include "../util/Matrix.h"

#define PARTICLES_PER_BILLBOARD 4

class Billboard {
private: 

public:
	Billboard();

	~Billboard();

    //Given a position, size and the current model view matrix, rotate() will populate pts with
    //the rotated coords in counterclockwise order.
    static void rotate(Vec3f* pts, Vec3f pos, float size, float modelview[16]);
};


#endif
