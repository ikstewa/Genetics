/*
	Moving Platform
	The base moving platform class

	Created By: Daniel White
*/

#ifndef _JUMP_PAD_H_
#define _JUMP_PAD_H_

#include "RenderedEntity.h"
using namespace::std;

class JumpPad : public RenderedEntity
{
public:
    JumpPad(Vec3f jF, int jD);
    ~JumpPad();
	void hitEntity(Entity* entity);
    Vec3f getJumpForce() const {return jumpForce;}
    void setJumpForce(Vec3f jF) {jumpForce = jF;}
    int getJumpDur() const {return jumpDur;}
    void setJumpDur(int jD) {jumpDur = jD;}

private:
    Vec3f jumpForce;
    int jumpDur; //in ms
};

#endif