/**
*
*/

#ifndef RENDERABLE_MODEL_H
#define RENDERABLE_MODEL_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include "AnimationManager.h"
#include "../tinyxml/tinyxml.h"
#include "../util/vec3.h"
#include "../util/Triangle.h"
#include "../world/AnimatedEntity.h"

struct UV
{
    UV(float inu, float inv) { u = inu; v = inv; }
    float u;
    float v;
};

class RenderableModel
{
public:
	RenderableModel();
	~RenderableModel();

    int getTriCount() const;
    float getRadius() const;
    Vec3f getPos() const;
    bool LoadModel(const char *filename);
    void Render();
    //void RenderOutline(bool); // DEPRECATED: 11/17/08 ~istewart
    Triangle* getTriangle(int index);

    void getArrays(Vec3f **v, Vec3f **n, UV **t, int *l, vector<Vec3i> *ind, vector<int>*, int *fl, map<int, int>*) const;

    Vec3f getMax() const;
    Vec3f getMin() const;

    map<int, int> texCount; // mapping of TexLib texID to triangle count

    // TEST ANIMATION FUNCTIONS, THIS SHOULD BE REMOVED
    void Animate(const AnimatedEntity *entity);
    void InitBoneSetup();

    static int showBone;

	std::vector<Vec3f> getVerts();
private:
	struct Bone
	{
		// Bone ID
		int id;
		// Bones position
		Vec3f pos;	
    Vec3f realPos;
		// Bones Rotation
		Vec3f rot;
    Vec3f tRot;
		// the influence pairs for the model
		std::vector< std::pair<int, float> > weights;
		// original distances from bone
		std::vector<Vec3f> originalDist;
		// our children
		std::vector<int> children;
	};

    // model extents
    float max_x, max_y, max_z, min_x, min_y, min_z;
    Vec3f center;

	// Vertex Data
	std::vector<Vec3f> verts;
	std::vector<Vec3f> originalVerts;
	
	// Normal data per vertices. 1:1 correlation
	std::vector<Vec3f> normals;
	// UV's for texture mapping
	std::vector<UV> uvs;
	// Face Data for all triangles
	std::vector<Vec3i> vertsPerFace;
	std::vector<Vec3i> uvsPerFace;
    std::vector<int> texPerFace;
    std::map<int, int> matMap;

	float time;

	// Bone data
	Bone *root;
	std::vector<Bone*> allBones;
  AnimationManager mAnimManager;
};

#endif