#ifndef __BSPNODE_H__
#define __BSPNODE_H__

#include "BSPElement.h"
#include <iostream>
#include <list>
#include "../render/Frustum.h"
using namespace std;

#define MAX_WEIGHT_PER_NODE         250
#define MAX_RATIO_BETWEEN_CHILDREN  1.5
#define MAX_SPLIT_ATTEMPTS          10

class BSPNode
{
public:
    BSPNode() : childA(NULL), childB(NULL) {depth = 0; radius = 0.0;}

    BSPNode(list<BSPElement *> *es, int w, int d);
    BSPNode(list<BSPElement *> *es);

    void init(list<BSPElement *> *es, int w);

    ~BSPNode();

    bool isLeaf();

    // returns all the elements below this node
    void getElements(list<BSPElement*> &elems);
    // get the elements near the region, more elimination done
    void getElements(Vec3f min, Vec3f max, list<BSPElement*> &elems);
    void getElements(Frustum* frustum, list<BSPElement*> &elems);
	
	//gets a bounding sphere of the elements in this node
	float getBoundingSphere(Vec3f *pos);

	//places pointers to children in the parameters
	void getChildren(BSPNode **cA, BSPNode **cB);


    // debug
    void printTree();
    void drawPlanes();
    void drawBoundingSpheres() const;
    int depth;

protected:

    Vec3f getCenterOfMass();

    int calcWeight();
	float calcRadius();
    bool shouldNodeSplit();
    void splitElements(float pLoc, int pA, list<BSPElement *> *listA, list<BSPElement *> *listB);
	void testSplitPlane(float pLoc, int pA, int *wA, int *wB, int *splitElements);
    void splitNode();

    list<BSPElement *> *elements;

    int weight;
    float planeLocation;
    int planeAxis;
	float radius;
	Vec3f center;

    BSPNode *childA;
    BSPNode *childB;
};

#endif
