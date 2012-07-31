#include "BSPNode.h"
#include <GL/glut.h>

BSPNode::BSPNode(list<BSPElement *> *es, int w, int d)
{
    childA = childB = NULL;
    depth = d;
	radius = 0.0;

    init(es, w);
}

BSPNode::BSPNode(list<BSPElement *> *es)
{
    childA = childB = NULL;
    depth = 0;
	radius = 0.0;

    init(es, -1);
}

BSPNode::~BSPNode()
{
    //if (elements)
    //  delete elements;

    if (childA)
        delete childA;
    if (childB)
        delete childB;
}

void BSPNode::init(list<BSPElement *> *es, int w)
{
    elements = es;

    weight = (w < 0 ? calcWeight() : w);
	radius = calcRadius();
	center = getCenterOfMass();

    if(shouldNodeSplit())
        splitNode();
}

float BSPNode::calcRadius() 
{
	float rad = 0.0;
	float dis;

   for (std::list<BSPElement *>::iterator itr = elements->begin();
         itr != elements->end(); itr++ )
   {
      BSPElement* be = *itr;
		dis = (be->getLocation() - center).length() + be->getRadius();
		if(dis > rad)
			rad = dis;
	}

	return rad;
}

bool BSPNode::shouldNodeSplit()
{
    return weight > MAX_WEIGHT_PER_NODE;
}

float BSPNode::getBoundingSphere(Vec3f *pos) 
{
	*pos = center;

	return radius;
}

void BSPNode::getChildren(BSPNode **cA, BSPNode **cB)
{
	*cA = childA;
	*cB = childB;
}

void BSPNode::getElements(list<BSPElement*> &elems)
{
    if (isLeaf())
    {
        elems.insert(elems.begin(), elements->begin(), elements->end());
        return;
    }

    childA->getElements(elems);
    childB->getElements(elems);
}

void BSPNode::getElements(Frustum* frustum, list<BSPElement*> &elems)
{
    // depth first search
    if (childA)
    {
        childA->getElements(frustum, elems);
        childB->getElements(frustum, elems);
        return;
    }

    // we're a leaf, check if were in frustum
    if (frustum->SphereInFrustum(&center, radius))
        elems.insert(elems.begin(), elements->begin(), elements->end());

}

void BSPNode::getElements(Vec3f min, Vec3f max, list<BSPElement*> &elems)
{
    float min_v, max_v;

    if (isLeaf())
    {
        elems.insert(elems.begin(), elements->begin(), elements->end());
        return;
    }
      

    switch(planeAxis)
    {
        // x
        case 0:
            min_v = min.x;
            max_v = max.x;
            break;
        // y
        case 1:
            min_v = min.y;
            max_v = max.y;
            break;
        // z
        case 2:
            min_v = min.z;
            max_v = max.z;
            break;
        // err
        default:
            break;
    }

    //// spans the plane
    //if(max_v >= planeLocation && min_v <= planeLocation)
    //   return elements;
    //else if(childA != NULL && max_v < planeLocation)
    //   return childA->getElements(min,max);
    //else if(childB != NULL && min_v > planeLocation)
    //   return childB->getElements(min,max);

    //// OPTIMIZATION ATTEMPT
    //// if we are overlapping a plane near the top try to further subdivide BOTH children
    //// spans the plane
    if(max_v >= planeLocation && min_v <= planeLocation)
    {
        childA->getElements(min, max, elems);
        childB->getElements(min, max, elems);
    }
    else if(childA != NULL && max_v <= planeLocation)
        childA->getElements(min, max, elems);
    else if(childB != NULL && min_v >= planeLocation)
        childB->getElements(min, max, elems);
    else
        printf("THIS SHOULDNT HAPPEN!?\n");


}


//list<BSPElement*>* BSPNode::getElements(Vec3f min, Vec3f max)
//{
//    float min_v, max_v;
//
//    if (isLeaf())
//        return elements;
//
//    switch(planeAxis)
//    {
//        // x
//        case 0:
//            min_v = min.x;
//            max_v = max.x;
//            break;
//        // y
//        case 1:
//            min_v = min.y;
//            max_v = max.y;
//            break;
//        // z
//        case 2:
//            min_v = min.z;
//            max_v = max.z;
//            break;
//        // err
//        default:
//            break;
//    }
//
//    // spans the plane
//    if(max_v >= planeLocation && min_v <= planeLocation)
//        return elements;
//    else if(childA != NULL && max_v < planeLocation)
//        return childA->getElements(min,max);
//    else if(childB != NULL && min_v > planeLocation)
//        return childB->getElements(min,max);
//
//    // should not happen??
//    printf("THIS SHOULDNT HAPPEN!?\n");
//    return NULL;
//}

void BSPNode::splitElements(float pLoc, int pA, list<BSPElement *> *listA, list<BSPElement *> *listB)
{
    float dis;

    // Populate the lists
    list<BSPElement *>::iterator itr;
    for(itr = elements->begin(); itr != elements->end(); itr++)
    {
        dis = (*itr)->elementVsPlane(pLoc,pA);

        if(dis <= 0)
            listA->push_back(*itr);

        if(dis >= 0)
            listB->push_back(*itr);
    }
}

void BSPNode::testSplitPlane(float pLoc, int pA, int *wA, int *wB, int *splitElements)
{
    float dis;

    // Populate the lists
    list<BSPElement *>::iterator itr;
    for(itr = elements->begin(); itr != elements->end(); itr++)
    {
        dis = (*itr)->elementVsPlane(pLoc,pA);

        if(dis <= 0)
            *wA += (*itr)->getWeight();

        if(dis >= 0)
            *wB += (*itr)->getWeight();

        if(dis == 0)
            *splitElements++;
    }
}

void BSPNode::splitNode()
{

    float pLoc;
    int pA = -1;
    int wA, wB;
    int elementsSplitByPlane;

    list<BSPElement *> *listA = new list<BSPElement *>;
    list<BSPElement *> *listB = new list<BSPElement *>;

    float offset = 0.0;
    for (int i = 0; i < MAX_SPLIT_ATTEMPTS; i++)
    {
        //printf("Atmpt #%d\n", i);
        wA = 0; wB = 0; elementsSplitByPlane = 0;

        // pick axis depending on depth
        switch ((depth%3 + i)%3)
        {
            case 0:
                pLoc = center.y + offset*i;
                pA = 1;
                break;
            case 1:
                pLoc = center.x + offset*i;
                pA = 0;
                break;
            case 2:
                pLoc = center.z + offset*i;
                pA = 2;
                break;
        }


        testSplitPlane(pLoc, pA, &wA, &wB, &elementsSplitByPlane);
        if (wA >= wB && (float)wA/(float)wB > MAX_RATIO_BETWEEN_CHILDREN)
        {
            // failed
            offset = -0.5f;
            pA = -1;
        }
        else if (wB >= wA && (float)wB/(float)wA > MAX_RATIO_BETWEEN_CHILDREN)
        {
            // failed
            offset = 0.5f;
            pA = -1;
        }
        else if (wA == weight || wB == weight)
        {
            // no improvement... now what?
            offset = 0.5f;
            pA = -1;
        }
        else
        {
            // success
            break;
        }
    }

    if (pA < 0)
    {
        //std::cout << "No valid split found! weight: " << weight << std::endl;
        delete listA;
        delete listB;
    }
    else
    {
        splitElements(pLoc, pA, listA, listB);

        childA = new BSPNode(listA, wA, depth+1);
        childB = new BSPNode(listB, wB, depth+1);

        planeLocation = pLoc;
        planeAxis = pA;

        weight = 0;

        // delete the array
        //TODO: TEMP DEBUG.. DO NOT CLEAR
        if (depth != 0)
        {
            elements->clear();
            delete elements;
        }
        elements = NULL;
    }
}

bool BSPNode::isLeaf()
{
    return (childA == NULL);
}

Vec3f BSPNode::getCenterOfMass()
{
    Vec3f ret = Vec3f(0,0,0);

    list<BSPElement *>::iterator i;
    for(i = elements->begin(); i != elements->end(); i++)
        ret = ret + (*i)->getLocation();

    ret = ret / (float)elements->size();

    return ret;
}

int BSPNode::calcWeight()
{
    int ret=0;

    list<BSPElement *>::iterator i;

    for(i = elements->begin(); i != elements->end(); i++)
        ret += (*i)->getWeight();

    return ret;
}

void BSPNode::printTree()
{
    if (!isLeaf())
    {
        //std::cout << "\t";
        childA->printTree();
        //std::cout << "\t";
        childB->printTree();
        //printf("NODE: depth[%d] weight[%d] planeLoc[%f] axis[%d] childA[%d] childB[%d]\n",
        //         depth, weight, planeLocation, planeAxis, childA, childB);
    }
    else
    {
		printf("LEAF: depth[%d] weight[%d] number of elements[%d]\n", depth, weight, elements->size());
    }
}

void BSPNode::drawPlanes()
{
    if (isLeaf())
        return;

    float extent = 50;

    glBegin(GL_QUADS);

    glColor3f(0.0,0.0,0.25*(1./depth));
    switch(planeAxis)
    {
        case 0:

            glVertex3f(planeLocation, -extent, -extent);
            glVertex3f(planeLocation, -extent,  extent);
            glVertex3f(planeLocation,  extent,  extent);
            glVertex3f(planeLocation,  extent, -extent);
            break;
        case 1:

            glVertex3f(-extent, planeLocation, -extent);
            glVertex3f(-extent, planeLocation,  extent);
            glVertex3f( extent, planeLocation,  extent);
            glVertex3f( extent, planeLocation, -extent);
            break;
        case 2:

            glVertex3f(-extent, -extent, planeLocation);
            glVertex3f(-extent,  extent, planeLocation);
            glVertex3f( extent,  extent, planeLocation);
            glVertex3f( extent, -extent, planeLocation);
            break;
    }
   
    glEnd();

    childA->drawPlanes();
    childB->drawPlanes();
}

void BSPNode::drawBoundingSpheres() const
{
    if (childA)
    {
        childA->drawBoundingSpheres();
        childB->drawBoundingSpheres();
        return;
    }

    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(center.x, center.y, center.z);
    glutSolidSphere(radius, 10, 10);
    glPopMatrix();

}
