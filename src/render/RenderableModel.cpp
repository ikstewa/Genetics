#include "RenderableModel.h"
#include "TextureLibrary.h"
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <queue>
#include "../util/Matrix.h"

#define ENABLE_CORRECTED_NORMALS 1

int RenderableModel::showBone = 0;

RenderableModel::RenderableModel()
{
	time = 0;
	root = 0;
}

RenderableModel::~RenderableModel()
{
    // TODO:: need to remove bones
    Bone *b;
    while(allBones.size())
    {
        b = allBones.back();
        allBones.pop_back();
        delete b;
    }
}

void RenderableModel::InitBoneSetup()
{
  if (!root)
		return;

	// update the vert positions
	Bone *current = root;
	std::queue<std::pair<Matrix, Bone*> > que;
  std::queue<Vec3f> parentQue;
  std::queue<Matrix> undoMat;
	Matrix ident;
	ident.MakeIdentity();
  //ident.RotY(3.14159);
	que.push(std::pair<Matrix, Bone*>(ident, root));
  parentQue.push(Vec3f());
  undoMat.push(ident);

	// loop in a breadth first manner
	while (que.size() > 0)
	{
		std::pair<Matrix, Bone*> pair = que.front();
		que.pop();
    Vec3f parentPos = parentQue.front();
    parentQue.pop();
    Matrix undo = undoMat.front();
    undoMat.pop();

		current = pair.second;
		current->originalDist = std::vector<Vec3f>(current->weights.size(), Vec3f());
		Matrix base, trans, rotX, rotY, rotZ;
		trans.Transform(current->pos);
		rotX.RotX(current->rot.x);
		rotY.RotY(current->rot.y);
		rotZ.RotZ(current->rot.z);

    // Create the base matrix for the bone
    base = pair.first * trans * rotZ * rotY * rotX ;

    // create inverse matrix for madness
    rotX.RotX(-current->rot.x);
    rotY.RotY(-current->rot.y);
    rotZ.RotZ(-current->rot.z);

    //Make the matrix to undo the init rotation of the bones
    Matrix tund = rotX * rotY * rotZ * undo;

    // get the vector from the bone to the vert to transform
		Vec3f bonePos = pair.first * current->pos;
    //current->pos = bonePos - parentPos;

		// Loop through all our influences
		for (unsigned int cnt=0; cnt < current->weights.size(); ++cnt)
		{
			Vec3f vPos = originalVerts[current->weights[cnt].first];
			
			//Vec3f dist = vPos - bonePos;
      Vec3f dist = vPos - bonePos;
      current->originalDist[cnt] = tund * dist;
      //current->originalDist[cnt] = tund * dist;
		}

		// add our children and loop
		for (unsigned int index=0; index < current->children.size(); ++index)
		{
			que.push(std::pair<Matrix, Bone*>(base, 
				allBones[current->children[index]]));
      parentQue.push(bonePos);
      undoMat.push(tund);
		}
	}
}

void RenderableModel::Animate(const AnimatedEntity *entity)
{
	if (!root || !entity)
		return;

	verts = originalVerts;
  //allBones[1]->rot.z = 3.14159/8;

	// update the vert positions
	Bone *current = root;
  std::vector<Vec3f> tempVerts(verts.size(), Vec3f());
	std::queue<std::pair<Matrix, Bone*> > que;
	Matrix ident;
	ident.MakeIdentity();
	que.push(std::pair<Matrix, Bone*>(ident, root));
  
  
	// loop in a breadth first manner
	while (que.size() > 0)
	{
		std::pair<Matrix, Bone*> pair = que.front();
		que.pop();
		current = pair.second;
		Matrix base, rot, trans, oriX, oriY, oriZ, rotX, rotY, rotZ;
		trans.Transform(current->pos);
		//trans.Invert();
    Vec3f boneRot = mAnimManager.GetBoneRot(entity, current->id);
    //Vec3f boneRot = current->rot;
    oriX.RotX(current->rot.x);
    oriY.RotY(current->rot.y);
    oriZ.RotZ(current->rot.z);
		rotX.RotX(boneRot.x);
		rotY.RotY(boneRot.y);
		rotZ.RotZ(boneRot.z);


		// make the composite matrix
    base = pair.first * trans * oriZ * oriY * oriX * rotZ * rotY * rotX;
    //rot = rotMat * oriZ * oriY * oriX * rotZ * rotY * rotX;

    Vec3f bonePos = pair.first * current->pos;
    
		// Loop through all our influences
		for (unsigned int cnt=0; cnt < current->weights.size(); ++cnt)
		{
			// multiply to get the new position
			Vec3f newPos = base * current->originalDist[cnt];
      
			tempVerts[current->weights[cnt].first] += 
				newPos*current->weights[cnt].second;
		}

		// add our children and loop
		for (unsigned int index=0; index < current->children.size(); ++index)
		{
			que.push(std::pair<Matrix, Bone*>(base, 
				allBones[current->children[index]]));
		}
	}

  verts = tempVerts;
}

Triangle* RenderableModel::getTriangle(int face)
{
    vector<Vec3f> v;

    Vec3i vert = vertsPerFace[face];

    v.push_back(verts[vert.x]);
    v.push_back(verts[vert.y]);
    v.push_back(verts[vert.z]);

    Triangle *t = new Triangle(v);

    t->uv[0] = Vec3f(uvs[uvsPerFace[face].x].u, uvs[uvsPerFace[face].x].v, 0.0);
    t->uv[1] = Vec3f(uvs[uvsPerFace[face].y].u, uvs[uvsPerFace[face].y].v, 0.0);
    t->uv[2] = Vec3f(uvs[uvsPerFace[face].z].u, uvs[uvsPerFace[face].z].v, 0.0);

    t->texID = matMap[texPerFace[face]];

    return t;
}

void RenderableModel::Render()
{
  

    TextureLibrary *texLib = TextureLibrary::getInstance();
    unsigned last = 0;

	for (unsigned int face = 0; face < vertsPerFace.size(); ++face)
	{
		Vec3i vert = vertsPerFace[face];
		Vec3i uv = uvsPerFace[face];
        if (matMap[texPerFace[face]] != last)
        {
            last = matMap[texPerFace[face]];
            texLib->Activate(matMap[texPerFace[face]]);
        }

        glBegin(GL_TRIANGLES);
		glTexCoord2d(uvs[uv.x].u, uvs[uv.x].v);
		glNormal3f(normals[vert.x].x,
			       normals[vert.x].y,
				   normals[vert.x].z);
		glVertex3f(verts[vert.x].x, 
			       verts[vert.x].y, 
				   verts[vert.x].z);

		glTexCoord2d(uvs[uv.y].u, uvs[uv.y].v);
		glNormal3f(normals[vert.y].x,
			       normals[vert.y].y,
				   normals[vert.y].z);
		glVertex3f(verts[vert.y].x, 
			       verts[vert.y].y, 
				   verts[vert.y].z);

		glTexCoord2d(uvs[uv.z].u, uvs[uv.z].v);
		glNormal3f(normals[vert.z].x,
			       normals[vert.z].y,
				   normals[vert.z].z);
		glVertex3f(verts[vert.z].x, 
			       verts[vert.z].y, 
				   verts[vert.z].z);
        glEnd();
	}
}

// DEPRECATED: 11/17/08 ~istewart
//void RenderableModel::RenderOutline(bool toggleSettings)
//{
//    // render outline
//
//    if (toggleSettings)
//    {
//        glCullFace(GL_FRONT);
//        glDisable(GL_LIGHTING);
//        glPolygonMode(GL_BACK, GL_LINE);
//        glDisable(GL_TEXTURE_1D);
//        glDisable(GL_TEXTURE_2D);
//        glDisable(GL_TEXTURE_3D);
//    }
//
//
//    glColor4f(0.0,0.0,0.0,1.0);
//    Render();
//
//
//    if (toggleSettings)
//    {
//        glEnable(GL_LIGHTING);
//        glCullFace(GL_BACK);
//        glPolygonMode(GL_BACK, GL_FILL);
//    }
//}

bool RenderableModel::LoadModel(const char *filename)
{
    TiXmlDocument doc(filename);
    if (doc.LoadFile(TIXML_ENCODING_UTF8))
    {
        TiXmlHandle hDoc(&doc);
        TiXmlHandle hRoot(0);
        TiXmlElement *pElem;

        // load materials
        TiXmlElement *mat = doc.FirstChildElement("Materials");

        if (mat)
        {
            for (pElem = mat->FirstChildElement();
                     pElem;
                     pElem = pElem->NextSiblingElement())
            {
                std::string val = pElem->Value();
                if (val.compare("Material") == 0)
                {
                    unsigned int id = TextureLibrary::getInstance()->Load(pElem->Attribute("file"));
                    int readID;
                    pElem->QueryIntAttribute("id", &readID);
                    matMap[readID] = id;
                    texCount[id] = 0;
                }
            }
        }

        // load model
        max_x = max_y = max_z = std::numeric_limits<float>::max()*(-1.0);
        min_x = min_y = min_z = std::numeric_limits<float>::max();

        hRoot = TiXmlHandle(doc.FirstChild("Model"));
        TiXmlElement *model = hRoot.Element();
        for (pElem = model->FirstChildElement();
             pElem;
             pElem = pElem->NextSiblingElement())
        {
            std::string type = pElem->Value();
            // if we are a vertex load
            if (type.compare("V") == 0)
            {
                float x, y, z;
                pElem->QueryFloatAttribute("x", &x);
                pElem->QueryFloatAttribute("y", &y);
                pElem->QueryFloatAttribute("z", &z);
                verts.push_back(Vec3f(x, y, z));

                // find max extents
                max_x = x > max_x ? x : max_x;
                max_y = y > max_y ? y : max_y;
                max_z = z > max_z ? z : max_z;
                min_x = x < min_x ? x : min_x;
                min_y = y < min_y ? y : min_y;
                min_z = z < min_z ? z : min_z;

                // now load normal
                pElem->QueryFloatAttribute("nx", &x);
                pElem->QueryFloatAttribute("ny", &y);
                pElem->QueryFloatAttribute("nz", &z);
                normals.push_back(Vec3f(x, y, z));
            }
            // load a face
            else if (type.compare("F") == 0)
            {
                int v1, v2, v3, tid;
                // load the vertex id's
                pElem->QueryIntAttribute("v1", &v1);
                pElem->QueryIntAttribute("v2", &v2);
                pElem->QueryIntAttribute("v3", &v3);
                vertsPerFace.push_back(Vec3i(v1, v2, v3));

                // load the uv indexes per face
                pElem->QueryIntAttribute("uv1", &v1);
                pElem->QueryIntAttribute("uv2", &v2);
                pElem->QueryIntAttribute("uv3", &v3);
                uvsPerFace.push_back(Vec3i(v1, v2, v3));

                // set texture
                pElem->QueryIntAttribute("tid", &tid);
                texPerFace.push_back(tid);
                texCount[matMap[tid]]++;
            }
            else if (type.compare("UV") == 0)
            {
                float u, v;
                pElem->QueryFloatAttribute("u", &u);
                pElem->QueryFloatAttribute("v", &v);
                uvs.push_back(UV(u, v));
            }
            // bones
            else if (type.compare("B") == 0)
            {
                Bone *bone = new Bone();
                allBones.push_back(bone);
                float x, y, z;
                int id;
                pElem->QueryIntAttribute("id", &id);
                pElem->QueryFloatAttribute("x", &x);
                pElem->QueryFloatAttribute("y", &y);
                pElem->QueryFloatAttribute("z", &z);

                // check to see if we are root
                if (id == 0)
                    root = bone;

                bone->id = id;
                bone->pos = Vec3f(x, y, z);
                pElem->QueryFloatAttribute("rx", &x);
                pElem->QueryFloatAttribute("ry", &y);
                pElem->QueryFloatAttribute("rz", &z);
                bone->rot = Vec3f(x, y, z);

                // Load the children id's and weights
                TiXmlElement *child = pElem->FirstChildElement();
                for (child; child; child = child->NextSiblingElement())
                {
                    std::string childType = child->Value();
                    if (childType.compare("Child") == 0)
                    {
                        int childID;
                        child->QueryIntAttribute("id", &childID);
                        bone->children.push_back(childID);
                    }
                    else if (childType.compare("Inf") == 0)
                    {
                       int vertID;
                       float amt;
                       child->QueryIntAttribute("id", &vertID);
                       child->QueryFloatAttribute("amt", &amt);
                       bone->weights.push_back(std::pair<int, float>(vertID, amt));
                    }
                }
            }
            else if (type.compare("Clip") == 0)
            {
                int id;
                pElem->QueryIntAttribute("id", &id);
                mAnimManager.LoadClip(pElem->Attribute("file"), id);
            }
        }
    }
    /*}
    else
    {
        std::cout << "Can't find or load model file: " << filename << std::endl;
    }*/

    center.x= (max_x-min_x)/2.0+min_x;
    center.y= (max_y-min_y)/2.0+min_y;
    center.z= (max_z-min_z)/2.0+min_z;

    /*
    // Center the model
    for (   std::vector<Vec3f>::iterator itr = verts.begin();
            itr != verts.end(); itr++ )
    {
        (*itr).x -= center.x;
        (*itr).y -= center.y;
        (*itr).z -= center.z;
    }*/
    
    //max_x += center.x;
    //max_y += center.y;
    //max_z += center.z;
    //min_x += center.x;
    //min_y += center.y;
    //min_z += center.z; 

    // Copy the verts into a seperate array
    originalVerts = verts;
    //GetOrignDist();
    InitBoneSetup();

    //// TEMP HACK TO FIX THE DAMN TEXTURES COORDS!
    //for (int i = 0; i < uvs.size(); i++)
    //{
    //    uvs[i].u = 0.5;
    //    uvs[i].v = 0.5;
    //}


	return true;
}

int RenderableModel::getTriCount() const
{
    return (int)vertsPerFace.size();
}

float RenderableModel::getRadius() const
{
    Vec3f pos = getPos();
    float dis,ret=0;    

    for each (Vec3f v in verts)
	  {
		dis = (v - pos).length();
        if(dis > ret)
            ret = dis;
	  }
    return ret;
}

Vec3f RenderableModel::getPos() const
{
    return center;
}

Vec3f RenderableModel::getMax() const
{
    return Vec3f(max_x, max_y, max_z);
}

Vec3f RenderableModel::getMin() const
{
    return Vec3f(min_x, min_y, min_z);
}

void RenderableModel::getArrays(Vec3f **v, Vec3f **n, UV **t, int *l, vector<Vec3i> *ind, vector<int> *tex, int *fl, map<int, int> *mat) const
{
    unsigned int i;

    unsigned int vertCount = (unsigned int)vertsPerFace.size() * 3;
    unsigned int faceCount = (unsigned int)vertsPerFace.size();

    *l = vertCount;     // vert count
    *fl = faceCount;    // face count
    *v = (Vec3f*)malloc(vertCount*sizeof(Vec3f)); // verts
    *n = (Vec3f*)malloc(vertCount*sizeof(Vec3f)); // normals
    *t = (UV*)malloc(vertCount*sizeof(UV));       // uv's

    for (i = 0; i < faceCount; i++)
    {
        // face to vert
        ind->push_back(Vec3i(3*i, 3*i+1, 3*i+2));
        // face to tex
        tex->push_back(texPerFace[i]);

        Vec3i vert = vertsPerFace[i];
        Vec3i uv = uvsPerFace[i];
        // verts
        Vec3f a = verts[vert.x];
        Vec3f b = verts[vert.y];
        Vec3f c = verts[vert.z];
        (*v)[3*i]   = Vec3f(a.x, a.y, a.z);
        (*v)[3*i+1] = Vec3f(b.x, b.y, b.z);
        (*v)[3*i+2] = Vec3f(c.x, c.y, c.z);
        // normals
#if ENABLE_CORRECTED_NORMALS
        Vec3f new_n = (*v)[3*i].getSurfNorm((*v)[3*i+1], (*v)[3*i+2]);
        (*n)[3*i]   = new_n;
        (*n)[3*i+1] = new_n;
        (*n)[3*i+2] = new_n;
#else
        (*n)[3*i]   = normals[vert.x];
        (*n)[3*i+1] = normals[vert.y];
        (*n)[3*i+2] = normals[vert.z];
#endif
        // uvs
        (*t)[3*i]   = uvs[uv.x];
        (*t)[3*i+1] = uvs[uv.y];
        (*t)[3*i+2] = uvs[uv.z];
    }
/*
    *l = verts.size();
    *v  = (Vec3f*)malloc(verts.size()*sizeof(Vec3f));
    *n  = (Vec3f*)malloc(normals.size()*sizeof(Vec3f));
    *t  = (UV*)malloc(uvs.size()*sizeof(UV));
    *fl = vertsPerFace.size();

    for(i=0;i<*l;i++)
        (*v)[i] = verts[i];
    for(i=0;i<*l;i++)
        (*n)[i] = normals[i];
    for(i=0;i<*l;i++)
        (*t)[i] = uvs[i];
    for(i=0;i<vertsPerFace.size();i++)
        ind->push_back(vertsPerFace[i]);
    for(i=0;i<vertsPerFace.size();i++)
        tex->push_back(texPerFace[i]);
*/

    // MatMap
    for (map<int,int>::const_iterator itr = matMap.begin();
        itr != matMap.end(); itr++)
    (*mat)[itr->first] = itr->second;


}

std::vector<Vec3f> RenderableModel::getVerts() {
	return verts;
}
