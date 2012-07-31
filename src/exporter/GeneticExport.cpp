/*
	Gentic Export
	A simple exporter for maya that can export models to a readable format
	that is usable by the Gentics Project.

	Created By: Greg Hoffman
	Copyright 2008
*/
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <maya/MSimple.h>
#include <maya/MFnSet.h>
#include <maya/MPlugArray.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnTransform.h>
#include <maya/MFnIkJoint.h>
#include <maya/MString.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItGeometry.h>
#include <maya/MObjectArray.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MPlug.h>
#include "../tinyxml/tinyxml.h"


DeclareSimpleCommand( GeneticExport, "", "8.5");

void ExportVerts(TiXmlElement *model, MFnMesh &mesh)
{
	// Export Vertices
	MFloatPointArray verts;
	mesh.getPoints(verts);
	
	int vertexCount = 0;
	for (int index = 0; index < verts.length(); ++index)
	{
		MVector normal;
		mesh.getVertexNormal(index, normal);
		TiXmlElement *newVert = new TiXmlElement("V");

		// output the position and normal
		newVert->SetDoubleAttribute("x", verts[index].x);
		newVert->SetDoubleAttribute("y", verts[index].y);
		newVert->SetDoubleAttribute("z", verts[index].z);
		newVert->SetDoubleAttribute("nx", normal.x);
		newVert->SetDoubleAttribute("ny", normal.y);
		newVert->SetDoubleAttribute("nz", normal.z);
		++vertexCount;

		model->LinkEndChild(newVert);
	}
}

void ExportMaterials(TiXmlElement *materials, MFnMesh &mesh)
{
	TiXmlElement *material = 0;
	MObjectArray shaders;
	MIntArray faceShaders;
	mesh.getConnectedShaders(0, shaders, faceShaders);

	// export all materials
	for (int i=0; i < shaders.length(); ++i)
	{
    MFnSet fnset(shaders[i]);
    MFnDependencyNode node(shaders[i]);
    MObject attr = node.attribute(MString("surfaceShader"));
    MPlug plug(shaders[i], attr);
    MPlugArray srcplugarray;
    plug.connectedTo( srcplugarray, true, false );

    if ( srcplugarray.length() == 0 ) continue;

    MObject matObj = srcplugarray[0].node();

     if ( matObj.hasFn( MFn::kPhong ))
     {
       MFnPhongShader shader(matObj);
       material = new TiXmlElement("Material");
       material->SetAttribute("name", shader.name().asChar());
       material->SetAttribute("id", i);
       material->SetAttribute("file", "");

       // grab the color


       // add it on to the last chain
       materials->LinkEndChild(material);
     }
		
	}
}

void ExportFaces(TiXmlElement *model, MFnMesh &mesh)
{
	MObjectArray shaders;
	MIntArray pvals, faceShaders;
	mesh.getConnectedShaders(0, shaders, faceShaders);

	// Export Faces
	for (int index = 0; index < mesh.numPolygons(); ++index)
	{
		mesh.getPolygonVertices(index, pvals);
		// loop through the list and print out all 3 verts as well as theUV's
		TiXmlElement *newFace = new TiXmlElement("F");
		newFace->SetAttribute("tid", faceShaders[index]);

		// This will erorr if not trianglated
		int uvid = -1;
		newFace->SetAttribute("v1", pvals[0]);
		newFace->SetAttribute("v2", pvals[1]);
		newFace->SetAttribute("v3", pvals[2]);

		// export UV's
		mesh.getPolygonUVid(index, 0, uvid);
		newFace->SetAttribute("uv1", uvid);
		mesh.getPolygonUVid(index, 1, uvid);
		newFace->SetAttribute("uv2", uvid);
		mesh.getPolygonUVid(index, 2, uvid);
		newFace->SetAttribute("uv3", uvid);

		// add to the xml chain
		model->LinkEndChild(newFace);
	}
}

void ExportUVS(TiXmlElement *model, MFnMesh &mesh)
{
	// Export UV's
	MFloatArray u;
	MFloatArray v;
	mesh.getUVs(u, v);
	for (int index = 0; index < u.length(); ++index)
	{
		TiXmlElement *newUV = new TiXmlElement("UV");
		newUV->SetDoubleAttribute("u", u[index]);
		newUV->SetDoubleAttribute("v", v[index]);
		model->LinkEndChild(newUV);
	}
}

void GetPlayer(MObject &model, MObject& bone, MObject &skin)
{
	/*
	MStatus stat;
	MItDag dagIter( MItDag::kBreadthFirst, MFn::kInvalid, &stat );

	for ( ; !dagIter.isDone(); dagIter.next())
	{
		MDagPath dagPath;
		stat = dagIter.getPath( dagPath );

		if ( stat )
		{
			MFnDagNode dagNode( dagPath, &stat );

			// if it is the mesh
			if (dagPath.hasFn( MFn::kMesh ))
			{
				// get the mesh and all its vertices
				MGlobal::displayInfo("Found mesh.");
				skin = dagPath.node();
			}
			else if (dagPath.hasFn(MFn::kJoint))
			{
				MGlobal::displayInfo("Found Joint.");
				bone = dagPath.node();
			}
			else if (dagPath.hasFn(MFn::kSkin))
			{
				MGlobal::displayInfo("Found Skin.");
				skin = dagPath.node();
			}
		}
	}
	*/
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);

	for (int i=0; i < selected.length(); i++)
	{
		MObject temp;
		selected.getDependNode(i, temp);
		if (temp.hasFn(MFn::kMesh))
		{
			MGlobal::displayInfo("Mesh Found.");
			selected.getDependNode(i, model);
		}
		else if (temp.hasFn(MFn::kJoint))
		{
			MGlobal::displayInfo("Joint Found.");
			selected.getDependNode(i, bone);
		}
		else if (temp.hasFn(MFn::kSkinClusterFilter))
		{
			MGlobal::displayInfo("Found Skin.");
			selected.getDependNode(i, skin);
		}
	}
}


/*
* EXPORT BONE
* Outputs the bones data to the file as well as recursively outputs its
* children. The bones are exported in a depth first manner. The file
* format is as follows:
* 
* B (BONE ID) (pos x) (pos y) (pos z) (rot x) (rot y) (rot z)
*   (num children) (children ID's)* (num influences) ((vertexID) (amount))*
* <B id="0" x="0.0" y="0.0" z="0.0" rx="0.0" ry="0.0" rz="0.0">
    <Child id="0"/>
	<Inf id="0.0" amt="0.0"/>
  </B>
*/

int ExportBones(MFnIkJoint &bones, MFnSkinCluster &skin, TiXmlElement *parent, int *boneCount)
{
	TiXmlElement *joint = new TiXmlElement("B");
	parent->LinkEndChild(joint);
	int boneID = *boneCount;
	*boneCount = *boneCount + 1;
	std::vector<int> childID;

	// Now do children
	for (int cnt = 0; cnt < bones.childCount(); ++cnt)
	{
		MFnIkJoint childJoint(bones.child(cnt));
		childID.push_back(ExportBones(childJoint, skin, parent, boneCount));
	}

	// get the bone attributes
	MVector pos = bones.getTranslation(MSpace::kTransform);
	MQuaternion rot, orient;
	bones.getRotation(rot);
	bones.getOrientation(orient);

	// Get final rotation and output pos and rotation
	//MQuaternion finalRot = rot + orient;
  MEulerRotation outRot = orient.asEulerRotation();

	// Set Position and rotation
	joint->SetAttribute("id", boneID);
	joint->SetDoubleAttribute("x", pos.x);
	joint->SetDoubleAttribute("y", pos.y);
	joint->SetDoubleAttribute("z", pos.z);
  joint->SetDoubleAttribute("rx", outRot.x);
	joint->SetDoubleAttribute("ry", outRot.y);
	joint->SetDoubleAttribute("rz", outRot.z);

	// Add Children
	for (int index=0; index < childID.size(); ++index)
	{
		TiXmlElement *child = new TiXmlElement("Child");
		joint->LinkEndChild(child);
		child->SetAttribute("id", childID[index]);
	}

	// Add Influences
	MDagPath skinPath;
	skin.getPathAtIndex(0, skinPath);
	MItGeometry gItr(skinPath);
	int cnt = 0;
	for (; !gItr.isDone(); gItr.next())
	{
		MDagPath p;
		MDoubleArray wts;
		bones.getPath(p);
		MStatus stat;
		unsigned int infcount = skin.indexForInfluenceObject(p, &stat);
		skin.getWeights(skinPath, gItr.component(), infcount, wts);

    if ( stat == MStatus::kFailure )
      MGlobal::displayInfo("Influence getting failed");

		// output the num of weights
		for (int i=0; i < wts.length() && stat != MStatus::kFailure; i++)
		{
			//if it doesn't add anything to this vertex ignore it
			if (wts[i] > 0.001f)
			{
				TiXmlElement *influence = new TiXmlElement("Inf");
				joint->LinkEndChild(influence);
				influence->SetAttribute("id", cnt++);
				influence->SetDoubleAttribute("amt", wts[i]);
			}
      else
      {
        cnt++;
      }
			 
		}
	}
	return boneID;
}

int ExportAnim(MFnIkJoint &bones, TiXmlElement *parent, int *boneCount)
{
	TiXmlElement *joint = new TiXmlElement("Bone");
	parent->LinkEndChild(joint);
	int boneID = *boneCount;
	*boneCount = *boneCount + 1;
	std::vector<int> childID;

	// Now do children
	for (int cnt = 0; cnt < bones.childCount(); ++cnt)
	{
		MFnIkJoint childJoint(bones.child(cnt));
		ExportAnim(childJoint, parent, boneCount);
	}

	// get the bone attributes
	MPlug rotXPlug = bones.findPlug("rx");
  MPlug rotYPlug = bones.findPlug("ry");
  MPlug rotZPlug = bones.findPlug("rz");
  MFnAnimCurve xcurve(rotXPlug);
  MFnAnimCurve ycurve(rotYPlug);
  MFnAnimCurve zcurve(rotZPlug);

  // loop through the curves and find what we need
  // all 3 attr must have a keyframe otherwise bad things happen
  for (unsigned int index = 0; index < xcurve.numKeys(); ++index)
  {
    double x = xcurve.evaluate(xcurve.time(index));
    double y = ycurve.evaluate(ycurve.time(index));
    double z = zcurve.evaluate(zcurve.time(index));

    MTime time = xcurve.time(index);
    double t = time.value();

    TiXmlElement *key = new TiXmlElement("Key");
    joint->LinkEndChild(key);
    key->SetAttribute("t", (int)t);
    key->SetDoubleAttribute("x", x);
    key->SetDoubleAttribute("y", y);
    key->SetDoubleAttribute("z", z);
  }

	// Set Position and rotation
	joint->SetAttribute("id", boneID);

	return boneID;
}

void ExportMapData(TiXmlElement *parent)
{
  MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);

  for (int i=0; i < selected.length(); i++)
	{
		MObject temp;
		selected.getDependNode(i, temp);
    if(temp.hasFn(MFn::kTransform))
    {
      MFnTransform trans(temp);
      MVector pos = trans.getTranslation(MSpace::kWorld);
      MPlug plug = trans.findPlug("Type");
      MString typeName;
      plug.getValue(typeName);
      
      // check the name of the type and output accordingly
      if (typeName == "Orb")
      {
        TiXmlElement *newOrb = new TiXmlElement("Orb");
        parent->LinkEndChild(newOrb);
        double val;
        int intval;

        // add ID for drawing
        plug = trans.findPlug("Id");
        plug.getValue(intval);
        newOrb->SetAttribute("id", intval); 

        // add position
        plug = trans.findPlug("translateX");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("x", val);
        plug = trans.findPlug("translateY");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("y", val);
        plug = trans.findPlug("translateZ");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("z",val);

        // add the orb type for what the orb does
        plug = trans.findPlug("OrbType");
        plug.getValue(intval);
        newOrb->SetAttribute("type", intval);

        // read in the data value to add
        plug = trans.findPlug("data");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("data", val);
      }
      else if (typeName == "JumpPad")
      {
        TiXmlElement *newOrb = new TiXmlElement("JumpPad");
        parent->LinkEndChild(newOrb);
        double val;
        int intval;

        // add ID for drawing
        plug = trans.findPlug("Id");
        plug.getValue(intval);
        newOrb->SetAttribute("id", intval); 

        // add position
        plug = trans.findPlug("translateX");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("x", val);
        plug = trans.findPlug("translateY");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("y", val);
        plug = trans.findPlug("translateZ");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("z",val);

        // add Force
        plug = trans.findPlug("JumpPad_UpForceX");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("fx", val);
        plug = trans.findPlug("JumpPad_UpForceY");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("fy", val);
        plug = trans.findPlug("JumpPad_UpForceZ");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("fz",val);

        // read in the data value to add
        plug = trans.findPlug("Duration");
        plug.getValue(val);
        newOrb->SetDoubleAttribute("dur", val);
      }
    }
  }
}
MStatus GeneticExport::doIt(const MArgList &args)
{
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);
	
	if (selected.length() == 0)
		return MS::kFailure;

	//MObject model;
	//selected.getDependNode(0, model);
	
	for (int argIndex = 0; argIndex < args.length(); ++argIndex)
	{
		if (args.asString(argIndex) == MString("-e"))
		{
			TiXmlDocument doc;
			TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
			doc.LinkEndChild(decl);
			TiXmlElement *model = new TiXmlElement("Model");
			TiXmlElement *materials = new TiXmlElement("Materials");

			MObject meshModel, bone, skin;

			GetPlayer(meshModel, bone, skin);

			MFnMesh fnMesh(meshModel);
			MFnIkJoint joint(bone);
			MFnSkinCluster fnSkin(skin);

			// Export Base Model
			ExportMaterials(materials, fnMesh);
			ExportVerts(model, fnMesh);
			ExportFaces(model, fnMesh);
			ExportUVS(model, fnMesh);
	
			int numBones = 0;
			ExportBones(joint, fnSkin, model, &numBones);

			// close output and save
			doc.LinkEndChild(materials);
			doc.LinkEndChild(model);
			doc.SaveFile("C:\\Character.tn");
			MGlobal::displayInfo("Output file");
		}
    else if (args.asString(argIndex) == MString("-m"))
    {
      TiXmlDocument doc;
			TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
			doc.LinkEndChild(decl);
      TiXmlElement *map = new TiXmlElement("Map");

      ExportMapData(map);

      doc.LinkEndChild(map);
      doc.SaveFile("C:\\Map.tn");
    }
    else if (args.asString(argIndex) == MString("-a"))
    {
      TiXmlDocument doc;
			TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
			doc.LinkEndChild(decl);
      TiXmlElement *clip = new TiXmlElement("Clip");
      doc.LinkEndChild(clip);

      // get the mobject for the bones
      MObject meshModel, bone, skin;

			GetPlayer(meshModel, bone, skin);
			MFnIkJoint joint(bone);
			
      int num = 0;
      ExportAnim(joint, clip, &num);
      doc.SaveFile("C:\\AnimClip.tn");
    }
	}


	return MS::kSuccess;
}