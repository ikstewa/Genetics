#include "Noise.h"

#define FP00 -0.5f
#define FP01 1.5f
#define FP02 -1.5f
#define FP03 0.5f
#define FP10 1.0f
#define FP11 -2.5f
#define FP12 2.0f
#define FP13 -0.5f
#define FP20 -0.5f
#define FP21 0.0f
#define FP22 0.5f
#define FP23 0.0f
#define FP30 0.0f
#define FP31 1.0f
#define FP32 0.0f
#define FP33 0.0f

Noise::Noise() {

}

void Noise::execute(ParticleSystem *particleSys, ParticleGroup &particleGroup) {
    static int frameCount = 0;
    float sinTheta, cosTheta, theta;
    float pathX, pathY, pathZ;
    float scalarX, scalarY, scalarZ;
    float noiseVal;
    ParticleList::iterator particles = particleGroup.begin();
    ParticleList::iterator particlesEnd = particleGroup.end();
    
    theta = (frameCount%600)*0.01047196f;
    cosTheta = cos(theta);
    sinTheta = sin(theta);

    pathX = sinTheta*0.005f*frameCount;
    pathY = 0.1215f*frameCount;
    pathZ = sinTheta*0.0035f*frameCount;
    
    scalarX = 0.5f+frameCount*0.010f;
    scalarZ = frameCount*0.0073f;

    while (particles != particlesEnd) {
        noiseVal = noise((*particles)->pos.x, (*particles)->pos.y, (*particles)->pos.z);

        (*particles)->pos.x -= pathX - noiseVal*scalarX;
        (*particles)->pos.y += -pathY + 0.5f*noiseVal;
        (*particles)->pos.z += pathZ - noiseVal*scalarZ;

        particles++;
    }


    frameCount++;
}

void Noise::execute(Particle &p, float dt) {
}

ParticleEffect* Noise::copy() {
    Noise *e = new Noise(*this);
    return e;
}

void Noise::valueTableInit(int seed) {
    int i;

    srand(seed);
    for(i=0; i<TABSIZE; i++) {
        valueTab[i] = 1.0f - 2.0f*pRandf();
    }
}

float Noise::lattice(int ix, int iy, int iz) {
    return valueTab[index(ix, iy, iz)];
}

float Noise::noise(float x, float y, float z) {
    int ix, iy, iz;
    int i, j, k;
    float fx, fy, fz;
    float xknots[4], yknots[4], zknots[4];
    static int initialized = 0;

    if (!initialized) {
       valueTableInit(seed);
       initialized = 1;
    }

    ix = floor(x);
    fx = x - ix;
    iy = floor(y);
    fy = y - iy;
    iz = floor(z);
    fz = z - iz;

    for (k=-1; k<=2; k++) {
        for (j=-1; j<=2; j++) {
            for (i=-1; i<=2; i++) {
                xknots[i+1] = lattice(ix+i, iy+j, iz+k);
            }
            yknots[j+1] = spline(fx, xknots);
        }
        zknots[k+1] = spline(fy, yknots);
    }

    return spline(fz, zknots);
}

float Noise::spline(float u, float *knots) {
    float c3, c2, c1, c0;

    c3 = FP00*knots[0] + FP01*knots[1] + FP02*knots[2] + FP03*knots[3];
    c2 = FP10*knots[0] + FP11*knots[1] + FP12*knots[2] + FP13*knots[3];
    c1 = FP20*knots[0] + FP21*knots[1] + FP22*knots[2] + FP23*knots[3];
    c0 = FP30*knots[0] + FP31*knots[1] + FP32*knots[2] + FP33*knots[3];

    return ((c3*u + c2)*u + c1)*u +c0;
}

int Noise::perm(int x) {
    return permTable[x&TABMASK];
}

int Noise::index(int ix, int iy, int iz) {
    return perm(ix+perm(iy+perm(iz)));
}

int Noise::floor(float x) {
    return (int)x;
}

void Noise::constructPermTable() {
    int used[TABSIZE];
    int i, num;
    
    for (i=0; i<TABSIZE; i++) {
        used[i] = 0;
    }

    for (i=0; i<TABSIZE; i++) {
        do {
            num = rand() % TABSIZE;
        }while(used[num] != 0);
        used[num] = 1;

        permTable[i] = num;
    }
}