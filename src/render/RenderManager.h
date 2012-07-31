/*
	Render Manager
	Manages the rendering functions of the game engine.

	Created By: Greg Hoffman
*/

#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#define SPHERE_IN_FRUSTRUM      0
#define SPHERE_ON_FRUSTRUM      1
#define SPHERE_OUTSIDE_FRUSTRUM 2

#include <map>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>
#include "RenderableModel.h"
#include "ShaderLibrary.h"
#include "Frustum.h"
#include "../BSP/BSPNode.h"
#include "../engine/Camera.h"
#include "../particle/ParticleSystemManager.h"
#include "../util/vec3.h"
#include "../util/Image.h"
#include "../util/Matrix.h"
#include "../util/Triangle.h"
#include "../util/MessageQueue.h"
#include "../world/RenderedEntity.h"
#include "../world/MovingPlatform.h"
#include "../world/Player.h"



class RenderManager
{
public:


    struct RenderableObject {
        RenderableModel *rm;
        RenderedEntity *e;
        //ShaderType shader;
    };

    /**
    * Constructor
    */
    RenderManager();

    bool show_fps;

    /**
    * RENDER
    * Renders the entire scene
    */
    void render(int vx, int vy, int vw, int vh, Camera* cam);
    void testRender(int vx, int vy, int vw, int vh, Camera* cam);

    /**
    * Setup
    * Sets up the opengl settings
    */
    void Setup();


    /**
    * Load Model
    * Loads a model into the system and adds it to the stack
    * @return True if succesful, else false
    * @param filename - the filename of the file to load
    */
    bool LoadModel(const char* filename, int rm_num);

    void AddEntity(RenderedEntity *e, int rm_num);
    
    void AddTerrain(RenderedEntity *e, int rm_num);

    // Creates a BB based off the loaded model
    AABoundingBox getModelBounds(int model_id);

    /**
    * Load Image
    * Loads an image into the render manager that can then be
    * used as a texture
    * @param filename - the filename of the file to load
    * @return True if the image is loaded else false
    */
    bool LoadImage(const char *filename, int *index);

    /**
    * Setup View
    * Sets up the open gl viewing code
    */
    void setupView(int x, int y, int w, int h, Camera*);

    /**
    * Build BSP Tree
    * builds the BSP tree, should be called after all models are laoded
    */
    void buildBSPTree();
  
    void setTree(BSPNode *tree, BSPNode *terrain);
    void setMessageQueue(MessageQueue *m) {MQ = m;}

    // Pre-renders the shadow map for the static terrain.
    // Sets up anythings else needed for shadows
    void setupShadows();

    short displayTexture;
    float threshold_d;
    float threshold_n;
// helper method for osd
    static void drawText(int x, int y, void * font, std::string text);
    void activateLossScreen();
protected:

    // fps vars
    int frame, my_time, timebase;
    char buffer[50];

    void fps();

    // Draws the textures onto the screen
    void drawScreen();

    void drawGUI();
    void drawMessages();
    void drawMenu();

    
    void drawHUD();

    /**
    * Cull Scene
    * Culls the list in bspTree to what is in the viewing frustrum and puts it in culledBSPElements
    */
    void cullScene();
    //helper for cullScene, recursively checks each node until a node's children are both in or touching the view frustrum
    //will return the node that should be drawn or further culled (ie each item in that nodes list tested vs view frustrum)
    //alternatively could return 0 if that particular node should not be drawn
    BSPNode *cullNodes(BSPNode *node);//, Matrix *M);

    Frustum *frustum;

    //------------------
    // Framebuffer shite
    GLuint fbo, fbo2;
    GLuint depthBuffer;//, depthBuffer2;
    GLuint imgTexture, depthTexture, normalTexture, gradTexture;
    void initFBO();
    void renderScene();
    void renderTerrain();
    void renderPlayer(bool anim);
    void renderGradient();

    //functions for setting up and drawing the particle systems.
    void setupParticleSystems();
    void drawParticleSystems();
	void updateParticleSystems();
    void drawCloudTest();
	//void drawCloudTest2();
	void setupCloudDLs();
    void drawAtom(Vec3f pos, Vec3f color);
	void drawFinalAtom(Vec3f pos, Vec3f color);
	void drawAtomCollect();
	void drawMessage(Vec3f pos);

    //-----------------
    // FBO
    void setupVBO();
    void putTrianglesInDrawList();

    //-----------------
    // Shadows
    void renderShadows();
    GLfloat cPM[16];
    GLfloat cVM[16];
    GLfloat lPM[16];
    GLfloat lVM[16];
    GLuint shadowMapTerrain;
    GLuint shadowColor;
    GLuint shadowFBO;
    float texMat[16];



private:

    bool lost;


    // All loaded models
    std::map<int,RenderableModel*> mAllModels;
    list<BSPElement*> mAllBSPElements;
    list<BSPElement*> mTerrainBSPElements;
    list<BSPElement*> culledBSPElements; //the 'working' list of objects to be rendered
    list<BSPElement*> culledTerrainBSPElements; //the 'working' list of objects to be rendered
    BSPNode *bspTree;
    BSPNode *terrainBspTree;
    RenderableObject *player;
    MessageQueue *MQ;

    //Particle system variables.
    ParticleSystemManager* pSysMan;
    std::vector<pPosTypeTuple> particlePosTypeList;
    int prevFrameTime;
    RenderableObject *cloudObj;
	GLuint cloudDL;
	RenderableObject *finalOrbObj;

    // VBO
    unsigned int textureVBOid;
    unsigned int verticesVBOid;
    unsigned int normalsVBOid;
    int m_nVertexCount;
    int m_nFaceCount;
    Vec3f *m_pVertices;
    Vec3f *m_pNormals;
    vector<Vec3i> vertsPerFace;
    vector<int> texPerFace;
    UV *m_pTexCoords;

    // support for VBO textures
    vector<unsigned int*> m_pFaceSet;
    vector<int> m_pFaceCountSet;
    map<int, int> matMap;
	float atomRot;
	Vec3f atomTrans;
};

#endif
