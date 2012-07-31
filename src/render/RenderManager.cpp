#include "RenderManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include "RenderableModel.h"
#include "ShaderLibrary.h"
#include "TextureLibrary.h"
#include "../world/AnimatedEntity.h"
#include "../GlobalSettings.h"

// All these light values are temporary till an actual light
// system is put in place
const GLfloat light_pos[4] = {500.0, 1000.0, 0.0, 1.0};
const GLfloat light_fov[2] = {800.0, 2050.0};
const GLfloat light_lap[4] = {0.0, 0.0, 150.0};
const GLfloat light_amb[4] = {0.6, 0.6, 0.6, 1.0};
const GLfloat light_diff[4] = {1.0, 0.8, 0.8, 1.0};
const GLfloat light_spec[4] = {0.8, 0.8, 0.8, 1.0};

#define INTERNAL_FORMAT GL_RGB10
#define INTERNAL_FORMAT2 GL_RGB10

// texture resolution
const int texWidth = 1024;
const int texHeight = 1024;
//const int texWidth2 = 1024;
//const int texHeight2 = 1024;

const int shadowMapStaticSize = 2048;
const float bMatrix[16] = {0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.5f, 0.0f,
                           0.5f, 0.5f, 0.5f, 1.0f};

//-----------------------------------------------------------------------------
// Constructor
RenderManager::RenderManager()
{
    show_fps = true;
    lost = false;
    displayTexture = 0;
    Setup();
}

void RenderManager::Setup()
{
    glLineWidth(4.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    //set up the diffuse, ambient and specular components for the light
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);

    // TODO: do we need better texcoord interpolation??
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    frustum = new Frustum();
    //MQ = new MessageQueue();

    #if ENABLE_PARTICLES
        setupParticleSystems();
    #endif

    #if ENABLE_FBO
        initFBO();
    #endif
}

void RenderManager::initFBO()
{
    TextureLibrary *texLib = TextureLibrary::getInstance();
    ShaderLibrary *shadeLib = ShaderLibrary::getInstance();
    // TODO: WHY NO MIPMAPS!?
    texLib->DisableMipmaps();

    // check the max supported color attachments
    //GLint maxBuffers;
    //glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxBuffers);
    //printf("RenderManager: MAX_COLOR_ATTACHMENTS[%i]\n", maxBuffers);

    // setup the fbo
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    //// setup color target
    //glGenTextures(1, &colorTextureID);
    //glBindTexture(texTargetType, colorTextureID);
    //glTexImage2D(texTargetType, 0, GL_RGBA8, colorTexWidth, colorTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //glTexParameterf(texTargetType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(texTargetType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameterf(texTargetType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(texTargetType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //
    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texTargetType, colorTextureID, 0);

    //// Create the depth target
    //glGenTextures(1, &depthTextureID);
    //glBindTexture(texTargetType, depthTextureID);
    //
    //glTexParameterf(texTargetType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameterf(texTargetType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(texTargetType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(texTargetType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameterf(texTargetType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //glTexImage2D(texTargetType, 0, GL_DEPTH_COMPONENT, depthTexWidth, depthTexHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, texTargetType, depthTextureID, 0);

    //glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);


    //// Attempt to use depth texture instead of renderbuffer
    //glGenTextures(1, &depthBuffer);
    //glBindTexture(GL_TEXTURE_2D, depthBuffer);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texWidth, texHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer, 0);

    // create the renderbuffer for depth
    glGenRenderbuffersEXT(1, &depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texWidth, texHeight);
    // bind the depth buffer
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

    // IMAGE TEXTURE
    imgTexture = texLib->Load("fake_file_ignore_me0");
    glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texLib->getGLID(imgTexture), 0);

    // DEPTH TEXTURE
    depthTexture = texLib->Load("fake_file_ignore_me1");
    glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, texLib->getGLID(depthTexture), 0);

    // NORMAL TEXTURE
    normalTexture = texLib->Load("fake_file_ignore_me2");
    glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, texLib->getGLID(normalTexture), 0);

    //// GRADIENT TEXTURE
    //gradTexture = texLib->Load("fake_file_ignore_me3");
    //glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT2, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_2D, texLib->getGLID(gradTexture), 0);

    // setup MRT
    GLenum mrt[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
    glDrawBuffers(3, mrt);


    // check FBO status
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        printf("RenderManager: ERROR: Could not setup FBO!\n");
        exit(1);
    }

    // unbind the FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    //------------------
    // GRADIENT FBO
    glGenFramebuffersEXT(1, &fbo2);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo2);

    // depth
    //glGenRenderbuffersEXT(1, &depthBuffer2);
    //glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer2);
    //glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, texWidth2, texHeight2);
    //glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer2);

    // GRADIENT TEXTURE
    gradTexture = texLib->Load("fake_file_ignore_me3");
    glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT2, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texLib->getGLID(gradTexture), 0);

    // check FBO status
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        printf("RenderManager: ERROR: Could not setup FBO2\n");
        exit(1);
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


    texLib->EnableMipmaps();

    //threshold_d = 0.009;
    //threshold_n = 0.001;
    threshold_d = 0.019;
    threshold_n = 0.375375;
}

void RenderManager::setupVBO()
{ 
    verticesVBOid = 0;
    textureVBOid = 0;
    normalsVBOid = 0;
    glGenBuffers( 1, &verticesVBOid );                      // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, verticesVBOid );     // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_nVertexCount*3*sizeof(float), m_pVertices, GL_STATIC_DRAW );

    // Generate And Bind The Texture Coordinate Buffer
    glGenBuffers( 1, &textureVBOid );                       // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, textureVBOid );      // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_nVertexCount*2*sizeof(float), m_pTexCoords, GL_STATIC_DRAW );

    glGenBuffers( 1, &normalsVBOid );                       // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, normalsVBOid );      // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_nVertexCount*3*sizeof(float), m_pNormals, GL_STATIC_DRAW );

    delete [] m_pVertices; m_pVertices = NULL;
    delete [] m_pNormals; m_pNormals = NULL;
    delete [] m_pTexCoords; m_pTexCoords = NULL;
}

void RenderManager::setupParticleSystems() 
{
    pSysMan = ParticleSystemManager::getInstance();
    pSysMan->setupParticleSystems(); //TODO: TEMPORARY!!!
    prevFrameTime = 0;
	atomRot = 0.0;
	atomTrans = Vec3f();
    cloudObj = new RenderableObject();
    Orb *orb = new Orb();

    cloudObj->e = orb;
    cloudObj->rm = NULL;

	finalOrbObj = new RenderableObject();
	orb = new Orb();
	finalOrbObj->e = orb;
	finalOrbObj->rm = NULL;

    pSysMan->setupMessage(Vec3f(), MESSAGE_JUMP_ORB, TEXTURE_JUMP_INCREASE);
}

//-----------------------------------------------------------------------------
// Models
bool RenderManager::LoadModel(const char *filename, int rm_num)
{
#ifdef VERBOSE
    printf("RenderManager: Loading Model[%s]...\n", filename);
#endif

    RenderableModel *model = new RenderableModel();
    if (model->LoadModel(filename))
    {
        //mAllModels.insert(rm_num,model);
        mAllModels[rm_num] = model;
        return true;
    }
    else
    {
        return false;
    }
}

AABoundingBox RenderManager::getModelBounds(int model_id)
{
    AABoundingBox b;
    RenderableModel* m = mAllModels[model_id];
    Vec3f minExtent = m->getMin();
    Vec3f maxExtent = m->getMax();

    float max = maxExtent.x - minExtent.x;
    max = max > maxExtent.z - minExtent.z ? max : maxExtent.z - minExtent.z;

    b.setWidth(max);
    b.setHeight(maxExtent.y - minExtent.y);
    b.setDepth(max);

    return b;
}


//-----------------------------------------------------------------------------
// Entities
void RenderManager::AddEntity(RenderedEntity *e, int rm_num)
{
    RenderableObject *re = new RenderableObject();
    re->rm = mAllModels[rm_num];
    re->e = e;

    if(re->e->getType() == ENTITY_TYPE_MOVING_PLATFORM)
        mAllBSPElements.push_back(
            new BSPElement( re, re->rm->getTriCount(),
                            ((MovingPlatform*)re->e)->getPathCenter(),
                            ((MovingPlatform*)re->e)->getRadius()));
    else if( re->e->getType() == ENTITY_TYPE_PLAYER )
        player = re;
    else //do not change this again.  I took the player out of the BSP tree on purpose.
    {
        mAllBSPElements.push_back(new BSPElement(re, re->rm->getTriCount(), re->e->getPos(), re->rm->getRadius()));
    }
    //debug
    //printf("pos %lf %lf %lf radius %lf\n",re->e->getPos().x,re->e->getPos().y,re->e->getPos().z,re->rm->getRadius());
}

void RenderManager::AddTerrain(RenderedEntity *e, int rm_num)
{
#ifdef VERBOSE
    printf("RenderManager: Creating terrain triangles...\n");
#endif
    RenderableModel *rm = mAllModels[rm_num];
    Triangle *t;

    for(int i=0;i<rm->getTriCount();i++)
    {
        t = rm->getTriangle(i);
        t->setIndex(i);
        mTerrainBSPElements.push_back(new BSPElement(t,1,t->getLocation(),t->getRadius()));
    }

    // setup the VBO
#if ENABLE_VBO
    rm->getArrays(&m_pVertices, &m_pNormals, &m_pTexCoords, &m_nVertexCount, &vertsPerFace, &texPerFace, &m_nFaceCount, &matMap);
    for (unsigned int i = 0; i < matMap.size(); i++)
        m_pFaceSet.push_back((unsigned int*)malloc(rm->texCount[matMap[i]] * 3 * sizeof(unsigned int)));
    setupVBO();

    putTrianglesInDrawList();
#endif

    // Remove the model now that we have triangles
    mAllModels.erase(rm_num);
    delete rm;
}


//-----------------------------------------------------------------------------
// Rendering
void RenderManager::render(int vx, int vy, int vw, int vh, Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vec3f lap = cam->getLookAt();
    Vec3f loc = cam->getLoc();
    Vec3f up = cam->getUp();
    gluLookAt(  loc.x, loc.y, loc.z, // eye location
                lap.x, lap.y, lap.z, // look at point
                up.x,  up.y,  up.z );// up vector

#if ENABLE_SHADOWS
    //glGetFloatv(GL_MODELVIEW_MATRIX, cVM);
    //cameraViewMatrix = Matrix(cVM);
    renderShadows();
#endif



    #if ENABLE_FBO
        //------------------------------
        // render the scene into the FBO
        //ShaderLibrary::getInstance()->attachShader(SHADER_MRT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0, 0, texWidth, texHeight);

            renderScene();

            glPopAttrib();

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        // second pass: gradient
        #if ENABLE_EDGE_DETECTION
        renderGradient();
        #endif

        //----------------------
        //Draw Particle Systems
        // draw particles after to avoid edge detection
        #if ENABLE_PARTICLES
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0, 0, texWidth, texHeight);

            //drawParticleSystems();
            //drawAtom(Vec3f(), Vec3f());
            for (unsigned int i=0; i<particlePosTypeList.size(); i++) {
                if (particlePosTypeList[i].type == ENTITY_TYPE_ORB) {
			        drawAtom(particlePosTypeList[i].pos, particlePosTypeList[i].color);
		        }else if (particlePosTypeList[i].type == ORB_TYPE_FINAL_ORB) {
			        drawFinalAtom(particlePosTypeList[i].pos, particlePosTypeList[i].color);
		        }
            }
            drawCloudTest();
            drawAtomCollect();
            updateParticleSystems();
            particlePosTypeList.clear();

            glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        #endif

        //--------------------------------
        // Draw the textures to the screen
        drawScreen();
    #else
        renderScene();
    #endif


    //----------------------------
    // Draw Messages
    //drawMessages();

    //----------------------------
    // Draw GUI elements
    #if ENABLE_HUD
    glUseProgram(0);
    drawGUI();
    #endif

    glutSwapBuffers();
}

void RenderManager::renderScene()
{
    //----------------------------
    // RENDER HERE
    RenderableObject * obj;
    ShaderType shader;
	Vec3f playerPos;

    //scene must be culled first
#if ENABLE_CULLING
    cullScene();
#endif


    ShaderLibrary *sl = ShaderLibrary::getInstance();
    TextureLibrary *tl = TextureLibrary::getInstance();

    // Draw the Geometry here.
    glPushMatrix();

        //---------------------------
        // LIGHT
        //set the light's position, needs to be changed
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

        #if DRAW_LIGHT_SOURCE
            glPushMatrix();
            glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
            //glDisable(GL_LIGHTING);
            sl->attachShader(SHADER_TOON_COLOR);
            glColor3f(1.0,1.0,0.0);
            glutWireSphere(1.0, 10, 10);
            //glEnable(GL_LIGHTING);
            glPopMatrix();
            glColor3f(1.0,1.0,1.0);
        #endif

        //---------------------------
        // CLOUD SHADER
        #if ENABLE_SHADER_CLOUDS
            // Draw the clouds
            glCullFace(GL_FRONT);
            ShaderLibrary::getInstance()->attachShader(SHADER_CLOUD);
            glutSolidSphere(3000, 10, 10);
            glCullFace(GL_BACK);
            glDisable(GL_TEXTURE_3D);
        #endif

        //-------------------
        // Render all models
        // TODO: should this be culledElements or all elements!?
        for each (BSPElement* bspelement in culledBSPElements)//mAllBSPElements)
        {
            shader = SHADER_TOON_TEXTURE;
            obj = ((RenderableObject*)bspelement->getElement());
            if( obj->e->isActive() )
            {
                Vec3f pos;
                AABoundingBox aabb;
                if(obj->e->getType() == ENTITY_TYPE_MOVING_PLATFORM)
                {
                    aabb = ((MovingPlatform*)obj->e)->getAABB();
                    pos = ((MovingPlatform*)obj->e)->getPos();
                }
                else
                {
                    aabb = obj->e->getAABB();
                    pos = obj->e->getPos();
                }

                
                if (obj->e->getType() == ENTITY_TYPE_ORB) 
                {
                    #if ENABLE_PARTICLES
                    pPosTypeTuple posType;

                    posType.pos = obj->e->getPos();
                    posType.type = ENTITY_TYPE_ORB;
					if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_ADD_TIME) {
                        posType.color = Vec3f(0.8, 0.3, 0.0);
					}else if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_INCR_JUMP) {
						posType.color = Vec3f(0.3, 0.8, 0.0);
					}else if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_FINAL_ORB) {
						posType.color = Vec3f(0.7, 0.3, 0.4);
						posType.type = ORB_TYPE_FINAL_ORB;
					/*else if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_ADD_TIME)
                        posType.color = Vec3f(1.0, 1.0, 1.0);
					else if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_ABILITY)
                        posType.color = Vec3f(1.0, 1.0, 1.0);
					else if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_INCR_JUMP)
                        posType.color = Vec3f(1.0, 1.0, 1.0);
					else if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_INCR_SPD)
                        posType.color = Vec3f(1.0, 1.0, 1.0);*/
					}else {
                        posType.color = Vec3f(0.8, 0.3, 0.0);
					}

                    particlePosTypeList.push_back(posType);
                    #else
                    // TODO: change me!
                    // Set the color based on orb type
                    shader = SHADER_TOON_COLOR;
                    if (((Orb*)(obj->e))->getOrbType() == ORB_TYPE_SLOW_TIME)
                        glColor3f(0.3, 0.8, 0.0);
                    else
                        glColor3f(0.8, 0.3, 0.0);

                    glPushMatrix();
                        glTranslatef(pos.x,pos.y,pos.z);
                        glRotatef(-obj->e->getDir(), 0, 1, 0);

                            sl->attachShader(shader);
                            obj->rm->Render();

                    glPopMatrix();
                    glColor3f(1.0,1.0,1.0);
                    #endif
                }
                else
                {
                    glPushMatrix();
                        glTranslatef(pos.x,pos.y,pos.z);
                        glRotatef(-obj->e->getDir(), 0, 1, 0);

                            sl->attachShader(shader);
                            obj->rm->Render();

                    glPopMatrix();
                }

                #if DRAW_BOUNDS
                    aabb.draw();
                #endif
            }
        }


        //----------------------
        // Draw terrain
        sl->attachShader(SHADER_TOON_TEXTURE);

        #if ENABLE_SHADOWS
            glActiveTexture(GL_TEXTURE2);

            // set the texture matrix
            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
            glLoadMatrixf(texMat);
            glMatrixMode(GL_MODELVIEW);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, shadowMapTerrain);

            glActiveTexture(GL_TEXTURE0);

            // enable the shadows
            GLint enableShadows = glGetUniformLocation(sl->shader[SHADER_TOON_TEXTURE]->shaderProg, "enableShadows");
            glUniform1f(enableShadows, 1.0);
        #endif
        
        renderTerrain();
        tl->Activate(TEXTURE_DISABLE);



        //----------------------
        // Draw Player
        renderPlayer(!ENABLE_SHADOWS);

        #if ENABLE_SHADOWS
            glMatrixMode(GL_TEXTURE);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glActiveTexture(GL_TEXTURE2);
            glDisable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            glDisable(GL_TEXTURE_2D);
            glUniform1f(enableShadows, 1.0);
        #endif

    glPopMatrix();

    tl->Activate(TEXTURE_DISABLE);
    sl->attachShader(SHADER_FIXED);

    #if !ENABLE_FBO && ENABLE_PARTICLES
    //drawParticleSystems();
    //drawAtom(Vec3f(), Vec3f());
	//drawMessage(playerPos);
    for (unsigned int i=0; i<particlePosTypeList.size(); i++) {
		if (particlePosTypeList[i].type == ENTITY_TYPE_ORB) {
			drawAtom(particlePosTypeList[i].pos, particlePosTypeList[i].color);
		}else if (particlePosTypeList[i].type == ORB_TYPE_FINAL_ORB) {
			drawFinalAtom(particlePosTypeList[i].pos, particlePosTypeList[i].color);
		}
    }
    drawCloudTest();
	drawAtomCollect();
    updateParticleSystems();
    particlePosTypeList.clear();
    #endif
}

void RenderManager::renderPlayer(bool anim)
{
    RenderableObject* obj = player;
    ShaderLibrary* sl = ShaderLibrary::getInstance();

    glPushMatrix();
    glTranslatef(obj->e->getPos().x,obj->e->getPos().y - obj->e->getAABB().getHeight()/2,obj->e->getPos().z);
    glRotatef(-obj->e->getDir() + 90, 0, 1, 0);

    #if ENABLE_CHARACTER_ANIM
    if( anim )
        obj->rm->Animate(dynamic_cast<AnimatedEntity*>(obj->e));
    #endif

    sl->attachShader(SHADER_TOON_TEXTURE);
    obj->rm->Render();//dynamic_cast<AnimatedEntity*>(obj->e));

    glPopMatrix();
    #if DRAW_BOUNDS
        obj->e->getAABB().draw();
    #endif

	#if ENABLE_PARTICLES
		drawMessage(obj->e->getPos());
	#endif
}

void RenderManager::renderTerrain()
{
    TextureLibrary* tl = TextureLibrary::getInstance();

    //----------------------
    // Render Terrain

    #if ENABLE_VBO
    // Enable Pointers
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Set pointers to our data
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBOid);
    glVertexPointer(3, GL_FLOAT, 0, (char *)NULL);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBOid);
    glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBOid);
    glNormalPointer(GL_FLOAT, 0, (char *)NULL);

    // Render
    tl->Activate(matMap[0]);
    for(unsigned int i = 0; i < m_pFaceSet.size(); i++, tl->Activate(matMap[i]))
    {
        glDrawElements(GL_TRIANGLES, m_pFaceCountSet[i]*3, GL_UNSIGNED_INT, m_pFaceSet[i]);
    }
    tl->Activate(TEXTURE_DISABLE);
    //glDrawElements(GL_TRIANGLES, m_pFacesCounter*3, GL_UNSIGNED_INT, m_pFaces);

    // Disable Pointers
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    #else
    unsigned int lastTex = 0;
    for each(BSPElement * el in culledTerrainBSPElements)
    {
        Triangle* tri = (Triangle*)el->getElement();
        vector<Vec3f> vert = tri->getVertices();

        // activate texture
        if (tri->texID != lastTex)
        {
            tl->Activate(tri->texID);
            lastTex = tri->texID;
        }

        glBegin(GL_TRIANGLES);
            glTexCoord2d(tri->uv[0].x, tri->uv[0].y);
            glNormal3f( tri->normal[0].x, 
                        tri->normal[0].y, 
                        tri->normal[0].z );
            glVertex3f( vert[0].x, vert[0].y, vert[0].z );

            glTexCoord2d(tri->uv[1].x, tri->uv[1].y);
            glNormal3f( tri->normal[1].x, 
                        tri->normal[1].y, 
                        tri->normal[1].z );
            glVertex3f( vert[1].x, vert[1].y, vert[1].z );

            glTexCoord2d(tri->uv[2].x, tri->uv[2].y);
            glNormal3f( tri->normal[2].x, 
                        tri->normal[2].y, 
                        tri->normal[2].z );
            glVertex3f( vert[2].x, vert[2].y, vert[2].z );
        glEnd();
    }
    #endif
}

void RenderManager::renderGradient()
{
    TextureLibrary* texLib = TextureLibrary::getInstance();
    ShaderLibrary* shadeLib = ShaderLibrary::getInstance();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // bind the FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo2);

    // setup the projection matrix
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0,0,texWidth, texHeight);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, texWidth, 0, texHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // render to the FBO
    // clear color??
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // enable the shader program
    shadeLib->attachShader(SHADER_GRADIENT);

    texLib->Activate(depthTexture);
    glActiveTexture(GL_TEXTURE1);
    texLib->Activate(normalTexture);

    // setup the uniforms
    Shader *s = shadeLib->shader[SHADER_GRADIENT];
    GLint w = glGetUniformLocation(s->shaderProg, "width");
    GLint h = glGetUniformLocation(s->shaderProg, "height");
    GLint dc = glGetUniformLocation(s->shaderProg, "depColor");
    GLint nc = glGetUniformLocation(s->shaderProg, "normColor");
    glUniform1f(w, texWidth);
    glUniform1f(h, texHeight);
    glUniform1i(dc, 0);
    glUniform1i(nc, 1);

    glBegin(GL_QUADS);

        //glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0, 0.0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(texWidth, 0.0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(texWidth, texHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0, texHeight);

    glEnd();

    texLib->Deactivate(normalTexture);
    glActiveTexture(GL_TEXTURE0);
    texLib->Deactivate(depthTexture);

    glUseProgram(0);

    // restore old viewport and projection
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glEnable(GL_DEPTH_TEST);
}

void RenderManager::drawScreen()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    //glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ShaderLibrary::getInstance()->attachShader(SHADER_FIXED);
    TextureLibrary *texLib = TextureLibrary::getInstance();
    texLib->Activate(TEXTURE_DISABLE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 100.0, 0.0, 100.0);
    //glOrtho(0, 100, 0, 100, -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    ShaderLibrary::getInstance()->attachShader(SHADER_EDGE);
    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    switch (displayTexture)
    {
        case 0:
        default:
            texLib->Activate(imgTexture);
            break;
        case 1:
            texLib->Activate(depthTexture);
            break;
        case 2:
            texLib->Activate(normalTexture);
            break;
    }
    glActiveTexture(GL_TEXTURE1);
    texLib->Activate(gradTexture);

    Shader* es = ShaderLibrary::getInstance()->shader[SHADER_EDGE];
    GLint wid = glGetUniformLocation(es->shaderProg, "width");
    GLint hei = glGetUniformLocation(es->shaderProg, "height");
    GLint thresh_d = glGetUniformLocation(es->shaderProg, "threshold_d");
    GLint thresh_n = glGetUniformLocation(es->shaderProg, "threshold_n");
    GLint img = glGetUniformLocation(es->shaderProg, "imgColor");
    GLint grad = glGetUniformLocation(es->shaderProg, "gradient");
    GLint edgeC = glGetUniformLocation(es->shaderProg, "edgeColor");
    GLint enable = glGetUniformLocation(es->shaderProg, "enableEdge");
    glUniform1i(enable, ENABLE_EDGE_DETECTION);
    glUniform1f(wid, texWidth);
    glUniform1f(hei, texHeight);
    glUniform1f(thresh_d, threshold_d);
    glUniform1f(thresh_n, threshold_n);
    glUniform4f(edgeC, 0.0, 0.0, 0.0, 0.0);
    glUniform1i(img, 0);
    glUniform1i(grad, 1);


    glTranslatef(0.0, 0.0, -0.5);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f( -0.5,  -0.5);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(100.0,  -0.5);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(100.5, 100.5);
        glTexCoord2f(0.0f, 1.0f); glVertex2f( -0.5, 100.5);
        //glTexCoord2f(0.0f, 0.0f); glVertex2f(  1.0,   1.0);
        //glTexCoord2f(1.0f, 0.0f); glVertex2f( 99.0,   1.0);
        //glTexCoord2f(1.0f, 1.0f); glVertex2f( 99.0,  99.0);
        //glTexCoord2f(0.0f, 1.0f); glVertex2f(  1.0,  99.0);
    glEnd();

    texLib->Deactivate(normalTexture);
    glActiveTexture(GL_TEXTURE0);
    texLib->Deactivate(imgTexture);
    ShaderLibrary::getInstance()->attachShader(SHADER_FIXED);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void RenderManager::setupView(int vx, int vy, int vw, int vh, Camera *cam)
{
    // setup the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // setup using a Camera object
    gluPerspective( cam->fov,       // FOV
                    (float)vw/vh,   // aspect
                    cam->zNear,     // near clipping plane
                    cam->zFar);     // far clipping plane

    glGetFloatv(GL_PROJECTION_MATRIX, cPM);
    //cameraProjectionMatrix = Matrix(cPM);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // setup the viewport
    glViewport(vx, vy, vw, vh);
}

void RenderManager::fps()
{
    drawText(86,96, GLUT_BITMAP_HELVETICA_12, "F11: fullscreen");
    drawText(86,94, GLUT_BITMAP_HELVETICA_12, "F12: exit");

    // Calculate the fps
    frame++;
    my_time = glutGet(GLUT_ELAPSED_TIME);
    if (my_time - timebase > 1000) {
        sprintf(buffer,"FPS: %4.2f",frame*1000.0/(my_time-timebase));
        timebase = my_time;
        frame = 0;
    }

    drawText(87,90, GLUT_BITMAP_HELVETICA_12, buffer);
}

void RenderManager::drawMessages()
{
    // ELIMINATED
    /*glDisable(GL_LIGHTING);
    glColor3f(0.0, 0.0, 1.0);
    draw message
    Message *m = MQ->getMessage();
    if(m==0)
        return;
    drawText(40,65, GLUT_BITMAP_HELVETICA_18, m->getString());*/
}

void RenderManager::drawGUI()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    //draw orb count
    int orbCount = ((Player*)(player->e))->getOrbCount();
    std::stringstream text;
    text << orbCount;
    glColor3f(0.0, 0.0, 1.0);
    drawText(80,5, GLUT_BITMAP_HELVETICA_18, text.str());
    //draw fps
    if (show_fps)
        fps();

    //draw time to live
    int t = (int)(((Player*)(player->e))->update());
    std::stringstream text2;
    text2 << t;
    //Move the timeleft numbers with the timeleft bar.
    float minX = 14.f;
    float maxX = 68.f;
    float timeLeft = ((Player*)(player->e))->getTimeToLive();
    float maxTime = ((Player*)(player->e))->getMaxTimeToLive();
    float xVal = minX + (timeLeft/maxTime * (maxX-minX));
    xVal = xVal>maxX ? maxX : xVal; //Don't allow the text to go past the maxX
    xVal = xVal<minX ? minX : xVal; //Don't allow the text to go past the minX
    drawText((int)xVal,6, GLUT_BITMAP_HELVETICA_12, text2.str());

    //draw HUD
    glColor3f(1.0, 1.0, 1.0);
    drawHUD();
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

void RenderManager::activateLossScreen()
{
    lost = true;
}

void RenderManager::drawHUD()
{
    // TODO: verify correct matrix mode return
    // remember the current matrix mode
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

    // Change the projection to a new Ortho for screen coord drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 100.0, 0.0, 100.0);

        // start with a fresh ModelView matrix to allow text drawing spontaneously
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glRasterPos2i(0,0);

            //Draw the timeleft frame
            /*
            glColor3f(1.f, 1.f, 1.f);
            TextureLibrary::getInstance()->Activate(TEXTURE_TIMELEFT_FRAME);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);	// Bottom Left Of The Texture and Quad
                glTexCoord2f(1.0f, 0.0f); glVertex2f( 100.0f, 0.0f);	// Bottom Right Of The Texture and Quad
                glTexCoord2f(1.0f, 1.0f); glVertex2f( 100.0f,  100.0f);	// Top Right Of The Texture and Quad
                glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,  100.0f);	// Top Left Of The Texture and Quad
            glEnd();
            //*/
            
            glColor3f(1.f, 1.f, 1.f);
            if( ((Player*)(player->e))->hasWon() )
            {
                TextureLibrary::getInstance()->Activate(TEXTURE_WIN);
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(20.f, 20.f);	// Bottom Left Of The Texture and Quad
                    glTexCoord2f(1.0f, 0.0f); glVertex2f( 80.f, 20.f);	// Bottom Right Of The Texture and Quad
                    glTexCoord2f(1.0f, 1.0f); glVertex2f( 80.f,  80.f);	// Top Right Of The Texture and Quad
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(20.f,  80.f);	// Top Left Of The Texture and Quad
                glEnd();
                TextureLibrary::getInstance()->Activate(TEXTURE_DISABLE);
            }
            else if( lost )
            {
                TextureLibrary::getInstance()->Activate(TEXTURE_LOSE);
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(25.f, 20.f);	// Bottom Left Of The Texture and Quad
                    glTexCoord2f(1.0f, 0.0f); glVertex2f(75.f, 20.f);	// Bottom Right Of The Texture and Quad
                    glTexCoord2f(1.0f, 1.0f); glVertex2f(75.f,  80.f);	// Top Right Of The Texture and Quad
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(25.f,  80.f);	// Top Left Of The Texture and Quad
                glEnd();
                TextureLibrary::getInstance()->Activate(TEXTURE_DISABLE);
            }
			else
			{
				//Create the timeleft bar through linear interpolation.
				float minX = 14.f;
				float maxX = 68.f;
				float timeLeft = ((Player*)(player->e))->getTimeToLive();
				float maxTime = ((Player*)(player->e))->getMaxTimeToLive();
				float xVal = minX + (timeLeft/maxTime * (maxX-minX));
				xVal = xVal>maxX ? maxX : xVal; //Don't allow the bar to get larger than maxX
				xVal = xVal<minX ? minX : xVal; //Don't allow the bar to get smaller than minX
				xVal = floor(xVal); //No rounding.
				//Draw the timeleft bar.
				//FIXME: for some reason this is showing up black.
				glColor3f(1.f, 0.f, 0.f);
				glBegin(GL_QUADS);
					glVertex2f(14.f,3.f); //Bottom left
					glVertex2f(xVal,3.f); //Bottom right
					glVertex2f(xVal,11.f); //Top right
					glVertex2f(14.f,11.f); //Top left
				glEnd();
			}

				//Draw the GUI background.
				glColor3f(1.f, 1.f, 1.f);
				TextureLibrary::getInstance()->Activate(TEXTURE_HUD2);
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);	// Bottom Left Of The Texture and Quad
					glTexCoord2f(1.0f, 0.0f); glVertex2f( 100.0f, 0.0f);	// Bottom Right Of The Texture and Quad
					glTexCoord2f(1.0f, 1.0f); glVertex2f( 100.0f,  100.0f);	// Top Right Of The Texture and Quad
					glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,  100.0f);	// Top Left Of The Texture and Quad
				glEnd();

        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
    glPopMatrix();


    // revert matrix_mode
    glMatrixMode(matrixMode);
    glMatrixMode(GL_MODELVIEW);
}


//-----------------------------------------------------------------------------
// Particles
void RenderManager::setupCloudDLs() {
	cloudDL = glGenLists(3);
	
	glNewList(cloudDL, GL_COMPILE);
		//glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		glScalef(1.03, 1.03, 1.03);
		cloudObj->rm->Render();
		//glDisable(GL_BLEND);
	glEndList();

	cloudDL++;
	glNewList(cloudDL, GL_COMPILE);
		glDepthMask(false);
		//glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glColor4f(0.0, 0.0, 0.0, 1.0);
		cloudObj->rm->Render();
		glDepthMask(true);
		//glDisable(GL_BLEND);
	glEndList();

	cloudDL++;
	glNewList(cloudDL, GL_COMPILE);
		//glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glColor4f(1.0, 1.0, 1.0, 1.0);
		cloudObj->rm->Render();		
		//glDisable(GL_BLEND);
	glEndList();

	cloudDL -= 2;
}


void RenderManager::drawCloudTest() {
	static bool first = true;
	static float rot = 0.0;
	//static float alpha = 1.0;
	static float inc = 3;
	TextureLibrary* textLib = TextureLibrary::getInstance();
	ShaderLibrary *sl = ShaderLibrary::getInstance();
	Vec3f pos, size;
	float alpha;

	rot += inc;

	//TODO: Dirty hack. Where is the constant for the rm_num of the ORB?
	if (cloudObj->rm == NULL) {
		cloudObj->rm = mAllModels[2];
	}

	if (first == true) {
		setupCloudDLs();
		first = false;
	}

    glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);		
	
	for (unsigned int i=5; i<pSysMan->getNumberParticleSystems(); i++) {
		pSysMan->setActiveParticleSystem(i);
		if (pSysMan->getType() != DUST_CLOUD_SYSTEM && pSysMan->getType() != CLOUD_SYSTEM) {
			continue;
		}

		if (pSysMan->getType() == DUST_CLOUD_SYSTEM) {
			glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }

		ParticleList::iterator particles = pSysMan->getParticleListBegin();
		ParticleList::iterator particlesEnd = pSysMan->getParticleListEnd();

		if (particles == particlesEnd) { //empty system
			pSysMan->deleteParticleSystem(i);
			i--;
			continue;
		}

		glPushMatrix();
		//glRotatef(20, 1.0, 0.0, 0.0);
		glDepthMask(false);
		while (particles != particlesEnd) { 		
			pos = (*particles)->pos;
			size = (*particles)->size;
			alpha = (*particles)->alpha;
			//draw the outline
			glPushMatrix();
				glColor4f(1.0-alpha, 1.0-alpha, 1.0-alpha, alpha);
				glTranslatef(pos.x, pos.y, pos.z);
				glRotatef(rot, 1.0, 0.0, 0.0);
				glScalef(size.x, size.y, size.z);
				sl->attachShader(SHADER_TOON_COLOR);
				glCallList(cloudDL);
			glPopMatrix();
			particles++;
		}
		glDepthMask(true);	

		particles = pSysMan->getParticleListBegin();
		particlesEnd = pSysMan->getParticleListEnd();
		while (particles != particlesEnd) { 
			pos = (*particles)->pos;
			size = (*particles)->size;
			alpha = (*particles)->alpha;
			//draw the mask
			glPushMatrix();
				glColor4f(1.0-alpha, 1.0-alpha, 1.0-alpha, alpha);
				glTranslatef(pos.x, pos.y, pos.z);
				glRotatef(rot, 1.0, 0.0, 0.0);
				glScalef(size.x, size.y, size.z);
				sl->attachShader(SHADER_TOON_COLOR);
				glCallList(cloudDL+1);
			glPopMatrix();

			//draw the sphere normally
			textLib->Activate(TEXTURE_DUST);
			glPushMatrix();
				glColor4f(1.0, 1.0, 1.0, alpha);
				glTranslatef(pos.x, pos.y, pos.z);
				glRotatef(rot, 1.0, 0.0, 0.0);
				glScalef(size.x, size.y, size.z);
				sl->attachShader(SHADER_TOON_COLOR);
				glCallList(cloudDL+2);	
			glPopMatrix();
			textLib->Deactivate(TEXTURE_DUST);

			particles++;
		}
		glPopMatrix();

		if (pSysMan->getType() == DUST_CLOUD_SYSTEM) {
			glDisable(GL_BLEND);
		}
	}
	
	glEnable(GL_LIGHTING);	

	sl->attachShader(SHADER_FIXED);
	/*if (alpha <= 0) {
		inc = 0.01;
	}else if (alpha >= 1.0){
		inc = -0.01;
	}
	alpha += inc;*/
}

void RenderManager::drawAtom(Vec3f pos, Vec3f color) {
	TextureLibrary* textLib = TextureLibrary::getInstance();
	ShaderLibrary *sl = ShaderLibrary::getInstance();
	ParticleSystemManager *pSysMan = ParticleSystemManager::getInstance();
	float rot[3], modelview[16];
	Vec3f coords[4];
	Vec3f particlePos, particleSize;
	int i;
	static float rotation = 0.0;
	rotation += 0.005;

	//TODO: Dirty hack. Where is the constant for the rm_num of the ORB?
	if (cloudObj->rm == NULL) {
		cloudObj->rm = mAllModels[2];
	}

	glPushMatrix();
	glTranslatef(atomTrans.x, atomTrans.y, atomTrans.z);
	sl->attachShader(SHADER_FIXED);
	glDisable(GL_LIGHTING);
	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	glDisable(GL_BLEND);
	//draw the center
	sl->attachShader(SHADER_TOON_COLOR);
	glColor4f(color.x, color.y, color.z, 0.95);
	pSysMan->setActiveParticleSystem(3); //HACK!!! need constants
	ParticleList::iterator particles = pSysMan->getParticleListBegin();
	ParticleList::iterator particlesEnd = pSysMan->getParticleListEnd();
	while (particles != particlesEnd) { 
		particlePos = (*particles)->pos;
		particleSize = (*particles)->size;
		glPushMatrix();	
			glTranslatef(pos.x+particlePos.x, pos.y+particlePos.y, pos.z+particlePos.z);
			glScalef(particleSize.x * 1.9, particleSize.y * 1.9, particleSize.z * 1.9);
			cloudObj->rm->Render();
		glPopMatrix();		
		particles++;
	}


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//Draw the rings
	sl->attachShader(SHADER_FIXED);
	textLib->Activate(TEXTURE_ATOM_RING);
	rot[0] = 0.0;
	rot[1] = 35.0;
	rot[2] = -35.0;

	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(atomRot, 0.0, 1.0, 0.0);
		for (i=0; i<3; i++) {
			glPushMatrix();	
				glRotatef(90, 0.0, 1.0, 0.0);
				glRotatef(rot[i], 0.0, 0.0, 1.0);		
				glScalef(1.2, 1.2, 1.2);
				glColor4f(color.x, color.y, color.z, 1.0);
				cloudObj->rm->Render();
			glPopMatrix();
		}
	glPopMatrix();
	
	textLib->Deactivate(TEXTURE_ATOM_RING);	
	glEnable(GL_CULL_FACE);	
	glDepthMask(true);

	//Draw the particles that orbit the rings
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);		
	for (i=0; i<3; i++) {
		pSysMan->setActiveParticleSystem(i);			
		//pSysMan->step();
		ParticleList::iterator pListBegin = pSysMan->getParticleListBegin();
		ParticleList::iterator pListEnd = pSysMan->getParticleListEnd();
		textLib->Activate((TextureId)pSysMan->getTextureId());

		glDepthMask(false); //Allow the particles to be drawn in any depth order.
		glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glRotatef(atomRot, 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
			while (pListBegin != pListEnd) { 
				(*pListBegin)->getBillboardedCoords(coords, modelview);

				glColor4d(color.x, color.y, color.z, (*pListBegin)->alpha);
				glBegin(GL_QUAD_STRIP);
				  glTexCoord2d(1,0); glVertex3f(coords[0].x, coords[0].y, coords[0].z);
				  glTexCoord2d(0,0); glVertex3f(coords[1].x, coords[1].y, coords[1].z);
				  glTexCoord2d(1,1); glVertex3f(coords[3].x, coords[3].y, coords[3].z);
				  glTexCoord2d(0,1); glVertex3f(coords[2].x, coords[2].y, coords[2].z);
				glEnd();
				
				pListBegin++;
			}
		glPopMatrix();
		glDepthMask(true);
		textLib->Deactivate((TextureId)pSysMan->getTextureId());
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	sl->attachShader(SHADER_FIXED);
	glPopMatrix();
}

void RenderManager::drawFinalAtom(Vec3f pos, Vec3f color) {
	TextureLibrary* textLib = TextureLibrary::getInstance();
	ShaderLibrary *sl = ShaderLibrary::getInstance();
	ParticleSystemManager *pSysMan = ParticleSystemManager::getInstance();
	float rot[3], modelview[16];
	Vec3f coords[4];
	Vec3f particlePos, particleSize;
	int i;
	static float rotation = 0.0;
	rotation += 0.005;

	//TODO: Dirty hack. Where is the constant for the rm_num of the ORB?
	if (finalOrbObj->rm == NULL) {
		finalOrbObj->rm = mAllModels[5];
	}

	glPushMatrix();
	glTranslatef(atomTrans.x, atomTrans.y, atomTrans.z);
	sl->attachShader(SHADER_FIXED);
	glDisable(GL_LIGHTING);
	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	glDisable(GL_BLEND);
	//draw the center
	sl->attachShader(SHADER_TOON_COLOR);
	glColor4f(color.x, color.y, color.z, 0.95);
	pSysMan->setActiveParticleSystem(8); //HACK!!! need constants
	ParticleList::iterator particles = pSysMan->getParticleListBegin();
	ParticleList::iterator particlesEnd = pSysMan->getParticleListEnd();
	while (particles != particlesEnd) { 
		particlePos = (*particles)->pos;
		particleSize = (*particles)->size;
		glPushMatrix();	
			glTranslatef(pos.x+particlePos.x, pos.y+particlePos.y, pos.z+particlePos.z);
			glScalef(particleSize.x * 1.9, particleSize.y * 1.9, particleSize.z * 1.9);
			finalOrbObj->rm->Render();
		glPopMatrix();		
		particles++;
	}


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//Draw the rings
	sl->attachShader(SHADER_FIXED);
	textLib->Activate(TEXTURE_ATOM_RING);
	rot[0] = 0.0;
	rot[1] = 35.0;
	rot[2] = -35.0;

	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(atomRot, 0.0, 1.0, 0.0);
		for (i=0; i<3; i++) {
			glPushMatrix();	
				glRotatef(90, 0.0, 1.0, 0.0);
				glRotatef(rot[i], 0.0, 0.0, 1.0);		
				glScalef(1.2, 1.2, 1.2);
				glColor4f(color.x, color.y, color.z, 1.0);
				finalOrbObj->rm->Render();
			glPopMatrix();
		}
	glPopMatrix();
	
	textLib->Deactivate(TEXTURE_ATOM_RING);	
	glEnable(GL_CULL_FACE);	
	glDepthMask(true);

	//Draw the particles that orbit the rings
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);		
	for (i=5; i<8; i++) { //magic numbers ftw
		pSysMan->setActiveParticleSystem(i);			
		//pSysMan->step();
		ParticleList::iterator pListBegin = pSysMan->getParticleListBegin();
		ParticleList::iterator pListEnd = pSysMan->getParticleListEnd();
		textLib->Activate((TextureId)pSysMan->getTextureId());

		glDepthMask(false); //Allow the particles to be drawn in any depth order.
		glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glRotatef(atomRot, 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
			while (pListBegin != pListEnd) { 
				(*pListBegin)->getBillboardedCoords(coords, modelview);

				glColor4d(color.x, color.y, color.z, (*pListBegin)->alpha);
				glBegin(GL_QUAD_STRIP);
				  glTexCoord2d(1,0); glVertex3f(coords[0].x, coords[0].y, coords[0].z);
				  glTexCoord2d(0,0); glVertex3f(coords[1].x, coords[1].y, coords[1].z);
				  glTexCoord2d(1,1); glVertex3f(coords[3].x, coords[3].y, coords[3].z);
				  glTexCoord2d(0,1); glVertex3f(coords[2].x, coords[2].y, coords[2].z);
				glEnd();
				
				pListBegin++;
			}
		glPopMatrix();
		glDepthMask(true);
		textLib->Deactivate((TextureId)pSysMan->getTextureId());
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	sl->attachShader(SHADER_FIXED);
	glPopMatrix();
}

void RenderManager::drawAtomCollect() {
	TextureLibrary* textLib = TextureLibrary::getInstance();
	ShaderLibrary *sl = ShaderLibrary::getInstance();
	ParticleSystemManager *pSysMan = ParticleSystemManager::getInstance();
	//Vec3f coords[4];
	Vec3f particlePos, particleSize, particleColor;
	Vec3f color = Vec3f(1.0, 1.0, 1.0);

	//TODO: Dirty hack. Where is the constant for the rm_num of the ORB?
	if (cloudObj->rm == NULL) {
		cloudObj->rm = mAllModels[2];
	}

	for (unsigned int i=5; i<pSysMan->getNumberParticleSystems(); i++) {
		pSysMan->setActiveParticleSystem(i);
		if (pSysMan->getType() != ORB_COLLECT_SYSTEM) {
			continue;
		}

		ParticleList::iterator particles = pSysMan->getParticleListBegin();
		ParticleList::iterator particlesEnd = pSysMan->getParticleListEnd();
		if (particles == particlesEnd) { //empty system
			pSysMan->deleteParticleSystem(i);
			i--;
			continue;
		}

		glPushMatrix();
		glTranslatef(atomTrans.x, atomTrans.y, atomTrans.z);
		sl->attachShader(SHADER_FIXED);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		//glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(false);

		//draw the center
		sl->attachShader(SHADER_TOON_COLOR);
		while (particles != particlesEnd) { 
			particlePos = (*particles)->pos;
			//printf("pos: %f, %f, %f\n", particlePos.x, particlePos.y, particlePos.z);
			particleSize = (*particles)->size;
			particleColor = (*particles)->color; 
			glPushMatrix();	
				glColor4f(particleColor.x, particleColor.y, particleColor.z, (*particles)->alpha);
				glTranslatef(particlePos.x, particlePos.y, particlePos.z);
				glScalef(particleSize.x, particleSize.y, particleSize.z);
				cloudObj->rm->Render();
			glPopMatrix();		
			particles++;
		}

		glDepthMask(true);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		sl->attachShader(SHADER_FIXED);
		glPopMatrix();
	}
}

void RenderManager::drawMessage(Vec3f pos) {
	TextureLibrary* textLib = TextureLibrary::getInstance();
	ParticleSystemManager *pSysMan = ParticleSystemManager::getInstance();
	Vec3f coords[4];
	float modelview[16];
	int type;
	size_t i, size;
	//Vec3f pos;
	
	size = pSysMan->getNumberParticleSystems();
	for (i=0; i<size; i++) {
		pSysMan->setActiveParticleSystem(i);
		type = pSysMan->getType();

		if (type != MESSAGE_JUMP_ORB && type != MESSAGE_TIME_ORB && type != MESSAGE_WIN && type != MESSAGE_LOSE) {
			continue; //not a message particle system
		}

		ParticleList::iterator particles = pSysMan->getParticleListBegin();
		ParticleList::iterator particlesEnd = pSysMan->getParticleListEnd();
		if (particles == particlesEnd) { //empty particle system
			pSysMan->deleteParticleSystem(i);
			return;
		}

        //printf("drawing message\n");
		//pos = (*particles)->pos;
		glPushMatrix();
		glTranslatef(pos.x, pos.y+3.4, pos.z);
		glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
		(*particles)->getBillboardedCoords(coords, modelview);

		textLib->Activate(pSysMan->getTextureId());

		glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        
		//glTranslatef(pos.x, pos.y, pos.z);
		//printf("drawing billboard at %f, %f, %f\n", pos.x, pos.y, pos.z);
        glColor4f(1.0, 1.0, 1.0, (*particles)->alpha);
        glScalef(10.0, 10.0, 10.0);
		glBegin(GL_QUAD_STRIP);
			glTexCoord2d(1,0); glVertex3f(coords[0].x, coords[0].y, coords[0].z);
			glTexCoord2d(0,0); glVertex3f(coords[1].x, coords[1].y, coords[1].z);
			glTexCoord2d(1,1); glVertex3f(coords[3].x, coords[3].y, coords[3].z);
			glTexCoord2d(0,1); glVertex3f(coords[2].x, coords[2].y, coords[2].z);
		glEnd();
        glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		glPopMatrix();

		textLib->Deactivate(pSysMan->getTextureId());
	}
}

void RenderManager::updateParticleSystems() {
	ParticleSystemManager* pSysMan = ParticleSystemManager::getInstance();
	unsigned int numSystems = (unsigned int)pSysMan->getNumberParticleSystems();
	int currFrameTime = glutGet(GLUT_ELAPSED_TIME);
	float timeStep = (currFrameTime - prevFrameTime) / 1000.0;
	float normalizedTime;
	prevFrameTime = currFrameTime;
	int cloudCount = 0;
	
	if ((currFrameTime / 2000) % 2 == 0) {//two second intervals
		normalizedTime = (currFrameTime % 2000) / 2000.0; 
	}else {
		normalizedTime = 1.0 - (currFrameTime % 2000) / 2000.0; 
	}
	
	atomTrans.y = 0.2 * cos(0.25*(2*PI*normalizedTime)) - 0.1; 

	atomRot += 0.2;
	if (atomRot >= 360.0) {
		atomRot = 0.0;
	}

	for (unsigned int i=0; i<numSystems; i++) {
		pSysMan->setActiveParticleSystem(i);
		if (pSysMan->getType() == CLOUD_SYSTEM) {
			cloudCount++;
		}

		if (i > 3 && (i < 5 || i > 8)) { //don't vary the timestep for the orbs
			pSysMan->setTimeStep(timeStep);
		}
		pSysMan->step();
	}

	PDBox box = PDBox(Vec3f(-200.0, 250.0, -200), Vec3f(200.0, 275, 200.0));
	while (cloudCount < 10) { //magic numbers make me sad
		pSysMan->setupCloud(box.Generate());
		cloudCount++;
	}

	glClearColor(0.53, 0.81, 1.0, 1.0);
}

void RenderManager::drawParticleSystems() {
	TextureLibrary* textLib = TextureLibrary::getInstance();
	size_t i;
	int ORB_PARTICLE_SYSTEMS = 1; //TEMPORARY!!!
	int ORB_START_IDX = 0;
	ShaderLibrary *sl = ShaderLibrary::getInstance();
	ShaderType shader = SHADER_TOON_TEXTURE;
	int currFrameTime = glutGet(GLUT_ELAPSED_TIME);

	float timeStep = (currFrameTime - prevFrameTime) / 1000.0;
	prevFrameTime = currFrameTime;

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND); // Enable blending so Alpha values will be used
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);		

	//TODO: Dirty hack. Where is the constant for the rm_num of the ORB?
	if (cloudObj->rm == NULL) {
		cloudObj->rm = mAllModels[2];
	}

    glPushMatrix();
	//glTranslated(0.0, 7.5, 5.0);
  
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	//update orb position
	pSysMan->setActiveParticleSystem(ORB_START_IDX);
	pSysMan->step();
	
	for (i=0; i<particlePosTypeList.size(); i++) {
		pPosTypeTuple posType = particlePosTypeList[i];
		Vec3f pos = posType.pos;
		//glTranslated(posType.pos.x, posType.pos.y, posType.pos.z);
		//printf("drawing particle at %f, %f, %f,\n", posType.pos.x, posType.pos.y, posType.pos.z);

		if (posType.type == ENTITY_TYPE_ORB) {
			for (int j=0; j<ORB_PARTICLE_SYSTEMS; j++) {
				pSysMan->setActiveParticleSystem(ORB_START_IDX+j);
				ParticleList::iterator pListBegin = pSysMan->getParticleListBegin();
				ParticleList::iterator pListEnd = pSysMan->getParticleListEnd();
				textLib->Activate((TextureId)pSysMan->getTextureId());

				glDepthMask(false); //Allow the particles to be drawn in any depth order.
				Vec3f coords[4];
				int count = 0;

				while (pListBegin != pListEnd) { 
					count++;
					(*pListBegin)->getBillboardedCoords(coords, modelview);

					glColor4d((*pListBegin)->color.x, (*pListBegin)->color.y, (*pListBegin)->color.z, (*pListBegin)->alpha);
					glBegin(GL_QUAD_STRIP);
					  glTexCoord2d(1,0); glVertex3f(pos.x+coords[0].x, pos.y+coords[0].y, pos.z+coords[0].z);
					  glTexCoord2d(0,0); glVertex3f(pos.x+coords[1].x, pos.y+coords[1].y, pos.z+coords[1].z);
					  glTexCoord2d(1,1); glVertex3f(pos.x+coords[3].x, pos.y+coords[3].y, pos.z+coords[3].z);
					  glTexCoord2d(0,1); glVertex3f(pos.x+coords[2].x, pos.y+coords[2].y, pos.z+coords[2].z);
					glEnd();

					
					pListBegin++;
				}
				glDepthMask(true);
				textLib->Deactivate((TextureId)pSysMan->getTextureId());
			}
		}
	}
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	//Draw the remaining systems.
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
    for (i=ORB_PARTICLE_SYSTEMS; i<pSysMan->getNumberParticleSystems(); i++) {
		//printf("Drawing particle system %d\n", i);
        pSysMan->setActiveParticleSystem(i);
		//pSysMan->setTimeStep(timeStep);
        pSysMan->step(); //update positions
		pSysMan->setSize(PDLine(Vec3f(0.3, 0.3, 0.3), Vec3f(0.8, 0.8, 0.8)));  //randomize the size of the particles
        ParticleList::iterator pListBegin = pSysMan->getParticleListBegin();
        ParticleList::iterator pListEnd = pSysMan->getParticleListEnd();
		textLib->Activate((TextureId)pSysMan->getTextureId());

        glDepthMask(false); //Allow the particles to be drawn in any depth order.
        Vec3f coords[4];
		int count = 0;

		/*if (pListBegin == pListEnd) { //empty particle system
			pSysMan->deleteParticleSystem(i);
			i--;
			continue;
		}*/

		Vec3f pos;
		Vec3f size;
		if (pListBegin == pListEnd) {
			pSysMan->deleteParticleSystem(i);
		}

        while (pListBegin != pListEnd) { 
			count++;
            //(*pListBegin)->getBillboardedCoords(coords, modelview);
			pos = (*pListBegin)->pos;
			size = (*pListBegin)->size;

            //glColor4d((*pListBegin)->color.x, (*pListBegin)->color.y, (*pListBegin)->color.z, (*pListBegin)->alpha);
            /*glBegin(GL_QUAD_STRIP);
		      glTexCoord2d(1,0); glVertex3f(coords[0].x, coords[0].y, coords[0].z);
              glTexCoord2d(0,0); glVertex3f(coords[1].x, coords[1].y, coords[1].z);
              glTexCoord2d(1,1); glVertex3f(coords[3].x, coords[3].y, coords[3].z);
			  glTexCoord2d(0,1); glVertex3f(coords[2].x, coords[2].y, coords[2].z);
            glEnd();*/

			//glTranslated(pos.x, pos.y, pos.z);
			//glutWireSphere(0.5, 10, 10);
			//sl->attachShader(SHADER_FIXED);
			//glutSolidSphere(0.5, 8, 8);
			
			//glTranslated(-pos.x, -pos.y, -pos.z); //translate back to the origin

			glPushMatrix();
                glTranslatef(pos.x,pos.y,pos.z);
				glScalef(size.x, size.y, size.z);
				glColor4f(1.0, 1.0, 1.0, (*pListBegin)->alpha);
                //glRotatef(-obj->e->getDir(), 0, 1, 0);

                sl->attachShader(shader);
                cloudObj->rm->Render();
            glPopMatrix();
            pListBegin++;
        }
        glDepthMask(true);
		textLib->Deactivate((TextureId)pSysMan->getTextureId());
    }
	
    glPopMatrix();
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Reset the blend func
    glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	
	//clear the particle list
	particlePosTypeList.clear();
	
    //End Particle Systems
	sl->attachShader(SHADER_FIXED);
}


//-----------------------------------------------------------------------------
// Culling
void RenderManager::cullScene()
{
    if(bspTree==NULL || terrainBspTree==NULL)
    {
        fprintf(stderr,"BSP Tree not generated before attempting to cull scene in Render Manager.\n");
        exit(-25);
    }

    // Setup the frustum
    frustum->CalculateFrustum();

    //empty previous list
    culledBSPElements.clear();
    culledTerrainBSPElements.clear();

    //--------------------------------------
    // Game objects

    //individually test each object in the node against viewing frustrum
    for each(BSPElement *be in mAllBSPElements)
    {
        //add element to list
        if(frustum->SphereInFrustum(&(be->getLocation()), be->getRadius()) || ((RenderableObject*)be->getElement())->e->getType() == ENTITY_TYPE_MOVING_PLATFORM)
            culledBSPElements.push_back(be);
        //else //debug
            //printf("element culled!\n");
    }
    //note to self: make sure 'flagged' invisible objects are not sent to render


    //----------------------------------------
    // Terrain

    //terrainBspTree->getElements(frustum, culledTerrainBSPElements);
    //printf("RenderManager: culledTerrain size[%d]\n", culledTerrainBSPElements.size());

    #if !ENABLE_VBO
    m_pFaceCountSet.clear();
    for (unsigned int i = 0; i < m_pFaceSet.size(); i++)
        m_pFaceCountSet.push_back(0);
    //individually test each object in the node against viewing frustrum
    for each(BSPElement *be in mTerrainBSPElements)
    {
        //add element to list
        if(frustum->SphereInFrustum(&(be->getLocation()), be->getRadius()))
        {

            culledTerrainBSPElements.push_back(be);

            //j = ((Triangle*)(be->getElement()))->getIndex();
            //// add the face to the proper set based of texID
            //int idx = texPerFace[j];
            //m_pFaceSet[idx][3*m_pFaceCountSet[idx]]   = vertsPerFace[j].x;
            //m_pFaceSet[idx][3*m_pFaceCountSet[idx]+1] = vertsPerFace[j].y;
            //m_pFaceSet[idx][3*m_pFaceCountSet[idx]+2] = vertsPerFace[j].z;
            //m_pFaceCountSet[idx]++;

        }
        //else //debug
            //printf("element culled!\n");
    }
    #endif

}

//-----------------------------------------------------------------------------
// Shadows
void RenderManager::testRender(int vx, int vy, int vw, int vh, Camera *cam)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    //glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ShaderLibrary::getInstance()->attachShader(SHADER_FIXED);
    TextureLibrary *texLib = TextureLibrary::getInstance();
    texLib->Activate(TEXTURE_DISABLE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    //gluOrtho2D(0.0, 100.0, 0.0, 100.0);
    //glOrtho(0, 100, 0, 100, -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glViewport(0, 0, vw, vh);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, shadowMapTerrain);

    glTranslatef(0.0, 0.0, -0.5);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, -1);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1, -1);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1,  1);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1,  1);
        //glTexCoord2f(0.0f, 0.0f); glVertex2f(  1.0,   1.0);
        //glTexCoord2f(1.0f, 0.0f); glVertex2f( 99.0,   1.0);
        //glTexCoord2f(1.0f, 1.0f); glVertex2f( 99.0,  99.0);
        //glTexCoord2f(0.0f, 1.0f); glVertex2f(  1.0,  99.0);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    //----------------------------
    // Draw GUI elements
    //#if ENABLE_HUD
    //drawGUI();
    //#endif

    glutSwapBuffers();

    return;




    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //// light projection
    //glMatrixMode(GL_PROJECTION);
    //glPushMatrix();
    //glLoadIdentity();
    //glLoadMatrixf(lPM);

    //// light view
    //glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    //glLoadIdentity();
    //glLoadMatrixf(lVM);

    ////Vec3f lap = cam->getLookAt();
    ////Vec3f loc = cam->getLoc();
    ////Vec3f up = cam->getUp();
    ////gluLookAt(  loc.x, loc.y, loc.z, // eye location
    ////            lap.x, lap.y, lap.z, // look at point
    ////            up.x,  up.y,  up.z );// up vector

    //// TODO: RENDER YOUR SHIT HERE!
    ////glDisable(GL_LIGHTING);
    //glShadeModel(GL_FLAT);
    ////glColorMask(0, 0, 0, 0);

    //renderTerrain();
    //glColorMask(1, 1, 1, 1);

    //glMatrixMode(GL_PROJECTION);
    //glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);
    //glPopMatrix();

    ////----------------------------
    //// Draw GUI elements
    //#if ENABLE_HUD
    //drawGUI();
    //#endif

    //glutSwapBuffers();
}
void RenderManager::setupShadows()
{
    glGenFramebuffersEXT(1, &shadowFBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowFBO);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &shadowMapTerrain);
    glBindTexture(GL_TEXTURE_2D, shadowMapTerrain);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(   GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                    shadowMapStaticSize, shadowMapStaticSize, 0,
                    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2DEXT(  GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                GL_TEXTURE_2D, shadowMapTerrain, 0);


    // create the shadow map texture
    //glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, shadowRB);
    //glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, shadowMapStaticSize, shadowMapStaticSize);
    //glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, shadowRB);
    glGenTextures(1, &shadowColor);
    glBindTexture(GL_TEXTURE_2D, shadowColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGBA,
                    shadowMapStaticSize, shadowMapStaticSize, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, shadowColor, 0);


   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
   {
      printf("ERROR: FBO status not complete!\n");
      exit(1);
   }

    //---------------
    // precalc the matrices

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // NOTE: Should be setup on resize/reshape
    // camera projection
    //glLoadIdentity();
    //gluPerspective( cam->fov,       // FOV
    //                (float)vw/vh,   // aspect
    //                cam->zNear,     // near clipping plane
    //                cam->zFar);     // far clipping plane
    //glGetFloatv(GL_MODELVIEW_MATRIX, cPM);
    //cameraProjectionMatrix = Matrix(cPM);

    // NOTE: Should be setup of redraw
    // camera view
    //glLoadIdentity();
    //Vec3f lap = cam->getLookAt();
    //Vec3f loc = cam->getLoc();
    //Vec3f up = cam->getUp();
    //gluLookAt(  loc.x, loc.y, loc.z, // eye location
    //            lap.x, lap.y, lap.z, // look at point
    //            up.x,  up.y,  up.z );// up vector
    //glGetFloatv(GL_MODELVIEW_MATRIX, cVM);
    //cameraViewMatrix = Matrix(cVM);

    // light projection
    glLoadIdentity();
    gluPerspective(45.0f, 1.0, light_fov[0], light_fov[1]);
    glGetFloatv(GL_MODELVIEW_MATRIX, lPM);

    // light view Matrix
    glLoadIdentity();;
    gluLookAt(  light_pos[0], light_pos[1], light_pos[2],
                light_lap[0], light_lap[1], light_lap[2],
                0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lVM);

    glPopMatrix();



    ////-------------------------------
    //// Render the terrain shadow map
    ////glClearDepth(1.0f);
    ////glDepthFunc(GL_LEQUAL);
    //glEnable(GL_DEPTH_TEST);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //// light projection
    //glMatrixMode(GL_PROJECTION);
    //glPushMatrix();
    //glLoadIdentity();
    //glLoadMatrixf(lPM);

    //// light view
    //glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    //glLoadIdentity();
    //glLoadMatrixf(lVM);

    //glPushAttrib(GL_VIEWPORT_BIT);
    //glViewport(0, 0, shadowMapStaticSize, shadowMapStaticSize);

    //// TODO: RENDER YOUR SHIT HERE!
    ////glDisable(GL_LIGHTING);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glShadeModel(GL_FLAT);
    //glColorMask(0, 0, 0, 0);
    ////glDisable(GL_DEPTH_TEST);

    //renderTerrain();

    ////glEnable(GL_LIGHTING);
    ////glDisable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    //glColorMask(1, 1, 1, 1);
    //glCullFace(GL_BACK);

    //glPopAttrib();

    //glMatrixMode(GL_PROJECTION);
    //glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);
    //glPopMatrix();

    //glDisable(GL_TEXTURE_2D);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // Clean up FBO and texture
    //glDeleteFramebuffersEXT(1, &shadowFBO);
    //glDeleteTextures(1, &imgTexture);

    // calculate the texture matrix
    // eye space to light clip space
    Matrix texMatrix = Matrix(bMatrix)*Matrix(lPM)*Matrix(lVM);
    texMatrix.asArray(texMat);
}

void RenderManager::renderShadows()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowFBO);
    //-------------------------------
    // Render the terrain shadow map
    //glClearDepth(1.0f);
    //glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // light projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glLoadMatrixf(lPM);

    // light view
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glLoadMatrixf(lVM);

    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, shadowMapStaticSize, shadowMapStaticSize);

    // TODO: RENDER YOUR SHIT HERE!
    //glDisable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_FLAT);
    glColorMask(0, 0, 0, 0);
    //glDisable(GL_DEPTH_TEST);

    renderTerrain();

    renderPlayer(true);

    //-----------------------
    // dynamic objects
    RenderableObject * obj;
    for each (BSPElement* bspelement in culledBSPElements)//mAllBSPElements)
    {
        obj = ((RenderableObject*)bspelement->getElement());
        if( obj->e->isActive() )
        {
            Vec3f pos;
            if(obj->e->getType() == ENTITY_TYPE_MOVING_PLATFORM)
                pos = ((MovingPlatform*)obj->e)->getPos();
            else
                pos = obj->e->getPos();

            glPushMatrix();
                glTranslatef(pos.x,pos.y,pos.z);
                glRotatef(-obj->e->getDir(), 0, 1, 0);

                obj->rm->Render();

            glPopMatrix();
        }
    }



    //glEnable(GL_LIGHTING);
    //glDisable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);
    glCullFace(GL_BACK);

    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // Clean up FBO and texture
    //glDeleteFramebuffersEXT(1, &shadowFBO);
    //glDeleteTextures(1, &imgTexture);

}
//-----------------------------------------------------------------------------
// Utility Methods
//-----------------------------------------------------------------------------
// Draws text to the screen. Can be invoked from anywhere in the display loop
// Specify color via glColor()
// NOTE: coords are from lower left screen. [0,100]
void RenderManager::drawText(int x, int y, void * font, std::string textString)
{
    // TODO: verify correct matrix mode return
    // remember the current matrix mode
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

    // remember the current lighitng status
    GLboolean flipLights;
    glGetBooleanv(GL_LIGHTING, &flipLights);



    // Change the projection to a new Ortho for screen coord drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 100.0, 0.0, 100.0);

        // start with a fresh ModelView matrix to allow text drawing spontaneously
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);

            glRasterPos2i(x,y);

            for (unsigned int index=0; index < textString.size(); ++index)
            {
                glutBitmapCharacter(font, textString[index]);
            }
          
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
    glPopMatrix();



    // return to the previous state
    //if (flipLights)
    //    glEnable(GL_LIGHTING);
    // revert matrix_mode
    glMatrixMode(matrixMode);
    //glMatrixMode(GL_MODELVIEW);
}

void RenderManager::setTree(BSPNode *tree, BSPNode *terrain)
{
    bspTree = tree;
    terrainBspTree = terrain;
}

void RenderManager::buildBSPTree()
{
#ifdef VERBOSE
    printf("RenderManager: Building BSP Tree...\n");
#endif
    bspTree->init(&mAllBSPElements, -1);
#ifdef VERBOSE
    printf("RenderManager: Building Terrain BSP Tree...\n");
#endif
    terrainBspTree->init(&mTerrainBSPElements, (int)mTerrainBSPElements.size());
    //debug
    //bspTree->printTree();
}

void RenderManager::putTrianglesInDrawList()
{
    // clear the face counters
    m_pFaceCountSet.clear();
    for (unsigned int i = 0; i < m_pFaceSet.size(); i++)
        m_pFaceCountSet.push_back(0);

    // add the face to the proper set based of texID
    for(unsigned int i=0; i < vertsPerFace.size(); i++)
    {
        int idx = texPerFace[i];
        m_pFaceSet[idx][3*m_pFaceCountSet[idx]]   = vertsPerFace[i].x;
        m_pFaceSet[idx][3*m_pFaceCountSet[idx]+1] = vertsPerFace[i].y;
        m_pFaceSet[idx][3*m_pFaceCountSet[idx]+2] = vertsPerFace[i].z;
        m_pFaceCountSet[idx]++;
    }
}
