
#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(BSPNode* gmObjs, BSPNode* terr)
{
    gameObjects = gmObjs;
    terrain = terr;
}

PhysicsEngine::PhysicsEngine()
{
    gameObjects = 0;
    terrain = 0;
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::update(vector<Entity*> el)
{
    //player(s) vs. entities/terrain hit detection
    updateExtraEntities(el);

    //TODO: entity vs. entity hit detection?

    //TODO: entity vs. terrain hit detection?

    //update positions using direction, velocity and force
    updatePositions(el);
}

void PhysicsEngine::hitDetectGameObjects(Entity* e, list<BSPElement*> &gameObjs)
{
    //printf("PhysicsEngine: hitDetectGameObjects count[%d]\n", gameObjs->size());
    //Grab the AABoundingBox from the entity.
    AABoundingBox eAABB = e->getAABB();

    //for each entity bsp tree node do hit detection.
    for each (BSPElement* bspE in gameObjs)
    {
        //Get the entity out of the RenderableObject struct, and get the AABB
        RenderManager::RenderableObject* bspRE_S = (RenderManager::RenderableObject*)(bspE->getElement());
        Entity* bspEntity = (Entity*)(bspRE_S->e);
        AABoundingBox bspEAABB;
        if( bspEntity->getType() == ENTITY_TYPE_MOVING_PLATFORM )
        {
            bspEAABB = ((MovingPlatform*)bspEntity)->getAABB();
        }
        else
        {
            bspEAABB = bspEntity->getAABB();
        }
        //AABoundingBox bspEAABB = (bspEntity->getType() == ENTITY_TYPE_MOVING_PLATFORM) ? ((MovingPlatform*)bspEntity)->getAABB() : bspEntity->getAABB();

        //check for hit, skip if inactive.
        if( bspEntity->isActive() && eAABB.intersects(bspEAABB) )
        {
            e->hitEntity(bspEntity);
        }
    }
}

void PhysicsEngine::hitDetectTerrainTriangles(Entity* e, list<BSPElement*> &terrainObjs)
{
    //printf("PhysicsEngine: hitDetectTerrainTriangles count[%d]\n", terrainObjs.size());
    //Grab the AABoundingBox from the entity.
    AABoundingBox eAABB = e->getAABB();

    //Calculate where one physics update puts the entity. NOTE: only approx. estimate.
    //For predictive hit checking!
    float timeSinceUpdate;
    bool hitGround=false;
    Vec3f p;
    Vec3f v;
    timeSinceUpdate = difftime(glutGet(GLUT_ELAPSED_TIME), e->getLastUpdateTime());
    p = eAABB.getCenter();
    v = e->getVel();
    p += v*(timeSinceUpdate/1000);

    //Declaration of needed variables.
    Hit hit;
    Ray ray;
    Triangle* tri;

    //default case
    e->setOffGround();

    //for each entity bsp tree node do hit detection.
    for each (BSPElement* bspE in terrainObjs)
    {
        //Get the Triangle out of the BSP element.
        tri = (Triangle*)(bspE->getElement());

        //Downward intersection w/ center of model
        //TODO: hit detection off all 4 AABB edges
        hit = Hit();
        ray = Ray(eAABB.getCenter(), Vec3f(0,-1,0));
        if( tri->intersect(ray,hit) )
        {
            //Calculate where the ray hit the traingle.
            Vec3f hitPoint = ray.getHitpoint(hit.nearIntersect);
            
            //Don't hit detect with triangles above the ray's original starting point. Greg says it can happen...
            if( hitPoint.y > eAABB.getCenter().y )
                continue;

            //if appropriate updated AABB plane is past the hitpoint (in the direction of the ray)
            if( (p.y-(eAABB.getHeight()/2.0)) <= hitPoint.y + 0.0001) //Only works because I know the ray is straight down.
            {
                //then call hitTerrain w/ ray.d and hitPoint (NOTE: perhaps just give it which AABB edge to set)
                e->setOnGround(); //set on ground because you've hit it, lol!
                e->hitTerrain(ray.d, hitPoint, *tri);
                hitGround=true;
            }
            else if(!hitGround && !((p.y-(eAABB.getHeight()/2.0)) > hitPoint.y) ) // else set off ground, because you're not hitting it, lol!
            {
                e->setOffGround();
            }
        }

        // Horizontal hit detection, in all four directions
        
        //+x
        Vec3f dir = Vec3f(1,0,0);
        dir.normalize();
        ray = Ray(eAABB.getCenter(), dir);
        hit = Hit();

        if( tri->intersect(ray,hit) )
        {
            Vec3f hitPoint = ray.getHitpoint(hit.nearIntersect);
            if( p.x+(eAABB.getWidth()/2.0) >= hitPoint.x )
            {
                e->hitTerrain(ray.d, hitPoint, *tri);
            }
        }

        //-x
        dir = Vec3f(-1,0,0);
        dir.normalize();
        ray = Ray(eAABB.getCenter(), dir);
        hit = Hit();

        if( tri->intersect(ray,hit) )
        {
            Vec3f hitPoint = ray.getHitpoint(hit.nearIntersect);
            if( p.x-(eAABB.getWidth()/2.0) <= hitPoint.x )
            {
                e->hitTerrain(ray.d, hitPoint, *tri);
            }
        }

        //+z
        dir = Vec3f(0,0,1);
        dir.normalize();
        ray = Ray(eAABB.getCenter(), dir);
        hit = Hit();

        if( tri->intersect(ray,hit) )
        {
            Vec3f hitPoint = ray.getHitpoint(hit.nearIntersect);
            if( p.z+(eAABB.getDepth()/2.0) >= hitPoint.z )
            {
                e->hitTerrain(ray.d, hitPoint, *tri);
            }
        }

        //-z
        dir = Vec3f(0,0,-1);
        dir.normalize();
        ray = Ray(eAABB.getCenter(), dir);
        hit = Hit();

        if( tri->intersect(ray,hit) )
        {
            Vec3f hitPoint = ray.getHitpoint(hit.nearIntersect);
            if( p.z-(eAABB.getDepth()/2.0) <= hitPoint.z )
            {
                e->hitTerrain(ray.d, hitPoint, *tri);
            }
        }
    }
}

void PhysicsEngine::updateExtraEntities(vector<Entity*> el)
{
    //Declare the culled versions of the BSP trees for game objects and terrain triangles.
    //NOTE: only declare because gameObjects and terrainObjects could be NULL.
    //list<BSPElement*>* gameObjs;
    //list<BSPElement*>* terrainObjs;

    //Iterate over all the extra entities (usually just one player, but could be more for multiplayer)
    for each (Entity* e in el)
    {
        //Skip inactive entities.

        if( !(e->isActive()) )
            continue;

        //for each game object to hit detection
        bspObjs.clear();
        //Grab culled version.
        gameObjects->getElements(e->getAABB().getMin(), e->getAABB().getMax(), bspObjs);
        hitDetectGameObjects(e, bspObjs);

        //for each terrain bsp tree node do hit detection.
        bspObjs.clear();
        //Grab culled version.
        terrain->getElements(e->getAABB().getMin(), e->getAABB().getMax(), bspObjs);
        hitDetectTerrainTriangles(e, bspObjs);
    }
}

void PhysicsEngine::updatePositions(vector<Entity*> el)
{
    /* FIXME: uncomment to have all game objects update, but for right now don't because the player is still in the game objects bsp tree.
    //create a list of all game objects to update.
    list<BSPElement*>* gameObjs = gameObjects->getElements();
    list<BSPElement*>::iterator itr;
    for (itr = gameObjs->begin(); itr != gameObjs->end(); itr++)
    {
        RenderManager::RenderableObject* bspRE_S = (RenderManager::RenderableObject*)((*itr)->getElement());
        Entity* bspEntity = (Entity*)(bspRE_S->e);
        el.push_back( bspEntity );
    }
    */

    float timeSinceUpdate;
    Vec3f p;
    Vec3f v;
    Vec3f a;
    vector<Force*> frcs;
    float m;
    for each (Entity* e in el)
    {
        p = e->getPos();
        v = e->getVel();
        a = e->getAcc();
        frcs = e->getForces();
        m = e->getMass();
        timeSinceUpdate = difftime(glutGet(GLUT_ELAPSED_TIME), e->getLastUpdateTime());

        //Physics Handwaving Shenanigans
        p += v*(timeSinceUpdate/1000);
        v += a*(timeSinceUpdate/1000);
        Vec3f allForces = Vec3f();
        vector<Force*>::iterator itr;
        for (itr=frcs.begin(); itr!=frcs.end(); itr++)
        {
            if( (*itr)->getDuration() >= time_t(timeSinceUpdate))
            {
                allForces += (*itr)->getForce();
                if( (*itr)->getDuration() != INT_MAX ) //INFINITE FORCES (like gravity)
                    (*itr)->setDuration((*itr)->getDuration() - time_t(timeSinceUpdate));
            }
            else if( (*itr)->getDuration() >= 0)
            {
                allForces += (*itr)->getForce() * (float)((*itr)->getDuration()) / timeSinceUpdate;
                if( (*itr)->getDuration() != INT_MAX ) //INFINITE FORCES (like gravity)
                    (*itr)->setDuration((*itr)->getDuration() - time_t(timeSinceUpdate));
            }
            else
            {
                e->removeForce(*itr);
            }
        }
        
        if( m > 0 )
            a = allForces/m;
        else
            a.zero();
        
        e->setPos(p);
        e->setVel(v);
        e->setAcc(a);
        e->setLastUpdateTime(glutGet(GLUT_ELAPSED_TIME));
    }
    //TODO: for each (BSPElement* bspE in *gameObjs)
}
