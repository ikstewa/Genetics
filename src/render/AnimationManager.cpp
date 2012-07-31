#include "AnimationManager.h"
#include "../tinyxml/tinyxml.h"
#include <string>

void AnimationManager::LoadClip(const char *filename, int id)
{
  std::vector<Curve<Vec3f>> boneKeys;
  float duration = 0;

  // load the file
  TiXmlDocument doc(filename);
  if (doc.LoadFile(TIXML_ENCODING_UTF8))
  {
    TiXmlHandle hDoc(&doc);
    TiXmlHandle hRoot(0);
    TiXmlElement *pElem;

    hRoot = TiXmlHandle(doc.FirstChild("Clip"));
    TiXmlElement *clip = hRoot.Element();
    clip->QueryFloatAttribute("duration", &duration);

    for (pElem = clip->FirstChildElement();
         pElem;
         pElem = pElem->NextSiblingElement())
    {
      std::string type = pElem->Value();
      if (type.compare("Bone") == 0)
      {
        // load all children keyframes
        Curve<Vec3f> boneAnim;

        // loop through all children of bone
        for (TiXmlElement *pKeys = pElem->FirstChildElement();
             pKeys;
             pKeys = pKeys->NextSiblingElement())
        {
          type = pKeys->Value();

          if (type.compare("Key") == 0)
          {
            int time;
            float x, y, z;
            pKeys->QueryIntAttribute("t", &time);
            pKeys->QueryFloatAttribute("x", &x);
            pKeys->QueryFloatAttribute("y", &y);
            pKeys->QueryFloatAttribute("z", &z);
            Vec3f p(x, y, z);
            boneAnim.AddPoint(p, p, p, time);
          }
        }

        boneKeys.push_back(boneAnim);
      }
    }
  }

  clipLib[id] = std::pair<std::vector<Curve<Vec3f>>, float>(boneKeys, duration);
}

Vec3f AnimationManager::GetBoneRot(const AnimatedEntity *entity, int boneid)
{
  Vec3f outRot;

  std::vector<AnimatedEntity::Clip> clips = entity->GetClips();

  // now go through the list of clips
  for (unsigned int index = 0; index < clips.size(); ++index)
  {
    int clipid = clips[index].clipid;
    std::pair<std::vector<Curve<Vec3f>>, float> current = clipLib[clipid];

    // get the percentage of the animation complete
    float length = clips[index].end - clips[index].start;
    length = clips[index].current / length;

    outRot += current.first.at(boneid).getValueAt(length);
  }
  // check to make sure we don't divide by 0
  
  if (clips.size() > 0)
    outRot = outRot/((float)clips.size()); 

  // export the new bone rot
  return outRot;
}