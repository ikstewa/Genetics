//-----------------------------------------------------------------------------
// Ryan Schmitt
// 10/14/08
// 
//
// Description:
//	The PhysicsEngine handles all hit detection and game Entity position
//	updates.
//-----------------------------------------------------------------------------


#ifndef __GENETICS_PHYSICS_ENGINE__
#define __GENETICS_PHYSICS_ENGINE__

#include <vector>
#include <time.h>
#include "../world/Entity.h"
#include "../BSP/BSPElement.h"
#include "../BSP/BSPNode.h"
#include "../util/Triangle.h"
#include "../render/RenderManager.h"
using namespace::std;

//-----------------------------------------------------------------------------
//
class PhysicsEngine
{

public:
    PhysicsEngine(BSPNode* gameObjects, BSPNode* terrain);
    PhysicsEngine();
    ~PhysicsEngine();

    void update(vector<Entity*> entityList);

protected:   

private:
    BSPNode* gameObjects;
    BSPNode* terrain;
    list<BSPElement*> bspObjs;
    void hitDetectGameObjects(Entity* e, list<BSPElement*> &gameObjs);
    void hitDetectTerrainTriangles(Entity* e, list<BSPElement*> &terrainObjs);
    void updateExtraEntities(vector<Entity*> entityList);
    void updatePositions(vector<Entity*> entityList);
};

#endif
