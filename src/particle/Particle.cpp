#include "Particle.h"

Particle::Particle(Vec3f vel, Vec3f pos, Vec3f color, Vec3f size, Vec3f rot, float alpha, float age,  
        float mass, float angularVel) : vel(vel), pos(pos), color(color), size(size), rot(rot), 
        alpha(alpha), age(age), mass(mass), angularVel(angularVel) {
}

Particle::Particle(const Particle& p) : vel(p.vel), pos(p.pos), color(p.color), size(p.size), rot(p.rot), 
        alpha(p.alpha), age(p.age), mass(p.mass), angularVel(p.angularVel) {
}

Particle::Particle() {
}

void Particle::getBillboardedCoords(Vec3f* pts, float modelview[16]) {
    Vec3f right(modelview[0], modelview[4], modelview[8]);
    Vec3f up(modelview[1], modelview[5], modelview[9]);

    pts[0].x = pos.x + (right.x+up.x)*-size.x;
    pts[0].y = pos.y + (right.y+up.y)*-size.y;
    pts[0].z = pos.z + (right.z+up.z)*-size.z;

    pts[1].x = pos.x + (right.x-up.x)*size.x;
    pts[1].y = pos.y + (right.y-up.y)*size.y;
    pts[1].z = pos.z + (right.z-up.z)*size.z;

    pts[2].x = pos.x + (right.x+up.x)*size.x;
    pts[2].y = pos.y + (right.y+up.y)*size.y;
    pts[2].z = pos.z + (right.z+up.z)*size.z;

    pts[3].x = pos.x + (up.x-right.x)*size.x;
    pts[3].y = pos.y + (up.y-right.y)*size.y;
    pts[3].z = pos.z + (up.z-right.z)*size.z;
}