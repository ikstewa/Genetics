

#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "../world/AnimatedEntity.h"
#include <map>
#include <vector>
#include "../util/curve.h"

class AnimationManager
{
public:
  
  Vec3f GetBoneRot(const AnimatedEntity *entity, int boneid);
  void LoadClip(const char *filename, int id);

private:
  // This confusing statement makes a map that has a id key
  // and a type that is a vector of 3d curves and a duration
  std::map<int, std::pair<std::vector<Curve<Vec3f>>, float>> clipLib;
};

#endif