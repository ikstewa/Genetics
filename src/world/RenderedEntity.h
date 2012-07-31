/*
	RenderedEntity
	Extends the base entity class to make it renderable by the game engine

	Created By: Greg Hoffman
*/

#ifndef _RENDEREDENTITY_H_
#define _RENDEREDENTITY_H_

#include "../util/vec3.h"
#include "Entity.h"

class RenderedEntity : public Entity
{
public:

	int GetRenderableID() { return renderID; }
	void SetRenderableID(int id) { renderID = id; }
private:
	int renderID;
};

#endif