//-----------------------------------------------------------------------------
// SoundManager.cpp
//
// Author:Dan White
// 08/2008
//
//-----------------------------------------------------------------------------
#include "SoundManager.h"

SoundManager* SoundManager::inst = 0;

SoundManager* SoundManager::getInstance()
{
    if (!inst)
        inst = new SoundManager();
    return inst;
}

SoundManager::SoundManager()
{
    // init FMOD sound system
   FSOUND_Init (44100, 32, 0);
}

void SoundManager::playMusic(unsigned int id)
{
    // play song
   FMUSIC_PlaySong(handles[id]);
}
void SoundManager::stopMusic(unsigned int id)
{
    // play song
   FMUSIC_StopSong(handles[id]);
}
unsigned int SoundManager::loadMusic(char *filename, bool loop, unsigned char volume)
{
    FMUSIC_MODULE* temp_handle;

    // load song
    temp_handle = FMUSIC_LoadSong (filename);
    handles.push_back(temp_handle);

    // play song only once
    // when you want to play a midi file you have to disable looping
    // BEFORE playing the song else this command has no effect!
    FMUSIC_SetLooping (temp_handle, loop);
    
    //set volume
    FMUSIC_SetMasterVolume (temp_handle, volume);

    return handles.size() - 1;
}

void SoundManager::playSound(unsigned int id)
{
    FSOUND_PlaySound (0,soundHandles[id]);
}

unsigned int SoundManager::loadSound(char *filename, bool loop, unsigned char volume)
{
    FSOUND_SAMPLE* temp_handle;

    // load song
    temp_handle = FSOUND_Sample_Load(FSOUND_FREE,filename,0, 0, 0);
    soundHandles.push_back(temp_handle);

    // play song only once
    // when you want to play a midi file you have to disable looping
    // BEFORE playing the song else this command has no effect!
    //FMUSIC_SetLooping (temp_handle, loop);
    
    //set volume
    //FSOUND_SetVolume(temp_handle, volume);

    return soundHandles.size() - 1;
}