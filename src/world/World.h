/** WORLD
* World Object that holds all the model data for the world
*/

#ifndef WORLD_H
#define WORLD_H

#include "../util/vec3.h"
#include "Entity.h"
#include <vector>
#include "../render/RenderManager.h"
#include "RenderedEntity.h"
#include "Player.h"
#include "../physics/PhysicsEngine.h"
#include "../engine/Camera.h"
#include "../util/MessageQueue.h"

class World
{
public:
    World(RenderManager *manager);
    ~World();

    /**
    * LoadScene
    * Loads the map from a scene file
    */
    bool LoadScene(const char *filename);

    /**
    * Add entity
    * Adds an entity to the list and adds a renderable
    * representation to the Render Manager
    */
    void AddEntity(RenderedEntity *e);

    /**
    * Remove Entity
    * Removes the entity from the world and a renderbale
    * representation from the Render Manager
    */
    void RemoveEntity(RenderedEntity *e);

    /**
    * Get Player
    * Returns the player character that is part of the world
    * @return the player object
    */
    Player* GetPlayer() { return player; }

    /**
    * Returns the current camera. Allows for dynamic camera types
    * @return The camera to render with
    */
    Camera* getCamera() const
    {
        // Change camera for cutscene/demo mode
        return player->getCamera();
    }

    void setMessageQueue(MessageQueue *m) {MQ = m; mManager->setMessageQueue(MQ);}

    /**
    * Update the world.
    */
    void updateWorld();

	void startClock() {player->startClock();}

private:

    RenderManager *mManager;

    // Entity Management
    std::vector<RenderedEntity*> mAllEntities;
    std::vector<Entity*> extraEntities;
    Player *player;
    PhysicsEngine physEngine;
    BSPNode bspTree;
    BSPNode bspTreeTerrain;
    MessageQueue *MQ;
};

#endif