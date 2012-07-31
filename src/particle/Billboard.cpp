#include "Billboard.h"

Billboard::Billboard() {
}

Billboard::~Billboard() {
}

void Billboard::rotate(Vec3f* pts, Vec3f pos, float size, float modelview[16]) {
    Vec3f right(modelview[0], modelview[4], modelview[8]);
    Vec3f up(modelview[1], modelview[5], modelview[9]);

    pts[0].x = (right.x+up.x)*-size;
    pts[0].y = (right.y+up.y)*-size;
    pts[0].z = (right.z+up.z)*-size;

    pts[1].x = (right.x-up.x)*size;
    pts[1].y = (right.y-up.y)*size;
    pts[1].z = (right.z-up.z)*size;

    pts[2].x = (right.x+up.x)*size;
    pts[2].y = (right.y+up.y)*size;
    pts[2].z = (right.z+up.z)*size;

    pts[3].x = (up.x-right.x)*size;
    pts[3].y = (up.y-right.y)*size;
    pts[3].z = (up.z-right.z)*size;
}
