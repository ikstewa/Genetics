#include "JumpPad.h"

//-----------------------------------------------------------------------------
// Constructor and Destructor
JumpPad::JumpPad(Vec3f jF, int jD)
{
	 jumpForce = jF;
     jumpDur = jD;
     setType(ENTITY_TYPE_JUMP_PAD);
}
JumpPad::~JumpPad()
{
}

//-----------------------------------------------------------------------------
// Hit Detection
void JumpPad::hitEntity(Entity* entity)
{
   //TODO: entity interaction
   //maybe limited number of uses? count down here until destruction?
}
