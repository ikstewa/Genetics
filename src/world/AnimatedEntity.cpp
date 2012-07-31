#include "AnimatedEntity.h"

void AnimatedEntity::AddClip(int id, float start, float end, float current)
{
   // if we have an existing clip return
   if(CheckForClip(id))
     return;

   Clip newClip;
   newClip.clipid = id;
   newClip.start = start;
   newClip.end = end;
   newClip.current = current;
   clips.push_back(newClip);
}

void AnimatedEntity::IncrementAnimation(float amt)
{
  // increment all the clips current values
  std::vector<Clip>::iterator itr;
  for (itr = clips.begin(); itr != clips.end(); itr++)
  {
    (*itr).current += amt;

    // check to see if the animation should be gone
    if ((*itr).current >= (*itr).end)
    {
      itr = clips.erase(itr);

      // check to see if that was the end
      if (itr == clips.end())
        break;
    }
  }
}

bool AnimatedEntity::CheckForClip(int id)
{
  std::vector<Clip>::iterator itr;
  for (itr = clips.begin(); itr != clips.end(); itr++)
  {
    if ((*itr).clipid == id)
      return true;
  }
  
  // didn't find it, return false
  return false;
}