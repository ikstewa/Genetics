//-----------------------------------------------------------------------------
// SoundManager.h
//
// Author:Dan White
// 08/2008
//
//-----------------------------------------------------------------------------

#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__


#include "fmod.h"
#include <vector>

using namespace std;

enum Sounds {
    SOUND_TIME_ORB = 0,
    SOUND_JUMP_ORB,
    SOUND_FINAL_ORB,
    SOUND_DEATH,
	SOUND_LOSE,
	SOUND_WIN,
	SOUND_LANDING_ENVIRONMENT,
	SOUND_LANDING_CHARACTER,
    SOUND_JUMP_PAD
};

enum Music {
    MUSIC_INTRO = 0,
    MUSIC_GAME
};

class SoundManager {

  public:
    static SoundManager* getInstance();

    void playMusic(unsigned int id);
    void stopMusic(unsigned int id);
    unsigned int loadMusic(char *filename, bool loop, unsigned char volume); //volume must be 0-255


    // sound files
    void playSound(unsigned int id);
    unsigned int loadSound(char *filename, bool loop, unsigned char volume);
  private:
    SoundManager();
    vector<FMUSIC_MODULE*> handles;
    vector<FSOUND_SAMPLE*> soundHandles;
    static SoundManager *inst;
};

#endif