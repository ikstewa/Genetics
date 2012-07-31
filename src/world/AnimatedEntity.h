/*
	AnimatedEntity
	Extends the drawable entity class to make it animatable

	Created By: Greg Hoffman
*/

#ifndef _ANIMATEDENTITY_H_
#define _ANIMATEDENTITY_H_

#include "../util/vec3.h"
#include "RenderedEntity.h"

#define ANIMATED_ENTITY_TYPE "ANIMATED_ENTITY"

class AnimatedEntity : public RenderedEntity
{
public:

  enum {
    ANIM_NULL,
    ANIM_WALKING,
    ANIM_IDLE,
    ANIM_RUNNING,
    ANIM_SIDESTEP_LEFT,
    ANIM_SIDESTEP_RIGHT,
    ANIM_WALKING_BACKWARDS,
    ANIM_JUMPING,
    ANIM_FALLING
  };

  // The clip struct is a representation of the animation manager
  // clips
  struct Clip
  {
    // The id of the clip inside the animation manager
    int clipid;
    // The start time of the clip (usually 0)
    float start;
    // The end time of the clip
    float end;
    // The current time of the clip
    float current;
  };

  void AddClip(int id, float start, float end, float current = 0);
  std::vector<Clip> GetClips() const { return clips; }
  void IncrementAnimation(float amt);

private:

  // checks to see if there is an existing clip present of a type
  bool CheckForClip(int id);

  std::vector<Clip> clips;
};

#endif