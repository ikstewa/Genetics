#include "World.h"

#include <iostream>
#include "../tinyxml/tinyxml.h"

World::World(RenderManager *manager)
{
    mManager = manager;
    mManager->setTree(&bspTree,&bspTreeTerrain);
    player = 0;
    physEngine = PhysicsEngine(&bspTree, &bspTreeTerrain);
}

World::~World()
{
    //TODO:: Need to remove all entities
}

void World::AddEntity(RenderedEntity *e)
{
    mAllEntities.push_back(e);

    // NOTE: The "terrain" entity no longer has an AABB
    if(e->getType()==ENTITY_TYPE_TERRAIN)
    {
        //terrain split into individual triangles
        mManager->AddTerrain(e, e->GetRenderableID());
        delete e;
    }
    else
    {
        mManager->AddEntity(e, e->GetRenderableID());
        e->setAABB(mManager->getModelBounds(e->GetRenderableID()));
    }
}

void World::RemoveEntity(RenderedEntity *e)
{
    // TODO:: need to do this...
}

bool World::LoadScene(const char *filename)
{
    TiXmlDocument doc(filename);
    if (doc.LoadFile(TIXML_ENCODING_UTF8))
    {
        TiXmlHandle hRoot(doc.FirstChild("Map"));
        TiXmlElement *map = hRoot.Element();
        for (TiXmlElement *pElem = map->FirstChildElement();
             pElem;
             pElem = pElem->NextSiblingElement())
        {
            std::string type = pElem->Value();
            if (type.compare("Geom") == 0)
            {
                int id = -1;
                pElem->QueryIntAttribute("id", &id);
                mManager->LoadModel(pElem->Attribute("file"), id);
            }
            else if (type.compare("Player") == 0)
            {
                int id = -1;
                float x, y, z;
                pElem->QueryIntAttribute("id", &id);
                pElem->QueryFloatAttribute("x", &x);
                pElem->QueryFloatAttribute("y", &y);
                pElem->QueryFloatAttribute("z", &z);
                player = new Player();

                // setup player
                player->SetRenderableID(id);
                player->setPos(Vec3f(x, y, z));
                player->setMass(5000);
                player->setAcc(Vec3f());
                player->setVel(Vec3f());
                player->setDir(-90);
                player->setMessageQueue(MQ);

                // add to the render manager so we can render
                //mManager->AddEntity(player, id);
                AddEntity(player);
                extraEntities.push_back(player);
            }
            else if (type.compare("Orb") == 0)
            {
                int id = -1;
                int type;
                float typeData;
                float x, y, z;
                pElem->QueryIntAttribute("id", &id);
                pElem->QueryFloatAttribute("x", &x);
                pElem->QueryFloatAttribute("y", &y);
                pElem->QueryFloatAttribute("z", &z);
                pElem->QueryIntAttribute("type", &type);
                pElem->QueryFloatAttribute("data", &typeData);
                Orb *orb = new Orb();
                orb->SetRenderableID(id);
                orb->setPos(Vec3f(x, y, z));
                orb->setVel(Vec3f());
                orb->setAcc(Vec3f());
                orb->setMass(0);
                orb->setOrbType(type);
                orb->setOrbTypeData(typeData);
                AddEntity(orb);
            }
            else if (type.compare("JumpPad") == 0)
            {
                int id = -1;
                float x, y, z;
                int jD;
                Vec3f jF;
                float fx, fy, fz;
                pElem->QueryIntAttribute("id", &id);
                pElem->QueryFloatAttribute("x", &x);
                pElem->QueryFloatAttribute("y", &y);
                pElem->QueryFloatAttribute("z", &z);
                pElem->QueryIntAttribute("dur", &jD); //force duration in ms
                pElem->QueryFloatAttribute("fx", &fx); //force magnitude
                pElem->QueryFloatAttribute("fy", &fy); //force magnitude
                pElem->QueryFloatAttribute("fz", &fz); //force magnitude
                jF = Vec3f(fx,fy,fz);
                JumpPad *jp = new JumpPad(jF,jD);
                jp->SetRenderableID(id);
                jp->setPos(Vec3f(x, y, z));
                jp->setVel(Vec3f());
                jp->setAcc(Vec3f());
                jp->setMass(0);
                AddEntity(jp);
            }
            else if (type.compare("MovingPlatform") == 0)
            {
                int id = -1;
                float sx, sy, sz;
                float ox, oy, oz;
                float ix, iy, iz;
                float ex, ey, ez;
                float w,l,h,s;  //s - speed
                pElem->QueryIntAttribute("id", &id);
                pElem->QueryFloatAttribute("sx", &sx);
                pElem->QueryFloatAttribute("sy", &sy);
                pElem->QueryFloatAttribute("sz", &sz);
                pElem->QueryFloatAttribute("ox", &ox);
                pElem->QueryFloatAttribute("oy", &oy);
                pElem->QueryFloatAttribute("oz", &oz);
                pElem->QueryFloatAttribute("ix", &ix);
                pElem->QueryFloatAttribute("iy", &iy);
                pElem->QueryFloatAttribute("iz", &iz);
                pElem->QueryFloatAttribute("ex", &ex);
                pElem->QueryFloatAttribute("ey", &ey);
                pElem->QueryFloatAttribute("ez", &ez);
                pElem->QueryFloatAttribute("w", &w);
                pElem->QueryFloatAttribute("l", &l);
                pElem->QueryFloatAttribute("h", &h);
                pElem->QueryFloatAttribute("s", &s);
                Vec3f sp = Vec3f(sx,sy,sz);
                Vec3f op = Vec3f(ox,oy,oz);
                Vec3f ip = Vec3f(ix,iy,iz);
                Vec3f ep = Vec3f(ex,ey,ez);
                MovingPlatform *mp = new MovingPlatform(sp,ip,op,ep,w,l,h,s);
                mp->SetRenderableID(id);
                //mp->setPos(Vec3f(x, y, z));
                mp->setVel(Vec3f());
                mp->setAcc(Vec3f());
                mp->setMass(0);
                AddEntity(mp);
            }
            else if (type.compare("LevelData") == 0)
            {
                int id = -1;
                pElem->QueryIntAttribute("id", &id);
                RenderedEntity *entity = new RenderedEntity();
                entity->SetRenderableID(id);
				entity->setType(ENTITY_TYPE_TERRAIN);
                AddEntity(entity);
            }
        }
        return true;
    }
    else
    {
        std::cout << "Unable to load map: " << filename << std::endl;
        return false;
    }
}

void World::updateWorld()
{
    physEngine.update(extraEntities);
}
