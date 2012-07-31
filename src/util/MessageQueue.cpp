#include "MessageQueue.h"
#include "../particle/ParticleSystemManager.h"
#include "../render/TextureLibrary.h"

MessageQueue* MessageQueue::inst = 0;

MessageQueue::MessageQueue()
{
}

MessageQueue* MessageQueue::getInstance()
{
    if (!inst)
        inst = new MessageQueue();
    return inst;
}

void MessageQueue::showMessage(unsigned int messageType, Vec3f position)
{
	ParticleSystemManager *pSysMan = ParticleSystemManager::getInstance(); 
    if( messageType == MESSAGE_JUMP_ORB )
    {
		pSysMan->setupMessage(position, MESSAGE_JUMP_ORB, TEXTURE_JUMP_INCREASE);
    }
    else if( messageType == MESSAGE_TIME_ORB )
    {
		pSysMan->setupMessage(position, MESSAGE_TIME_ORB, TEXTURE_TIME_INCREASE);
    }
    else if( messageType == MESSAGE_WIN )
    {
    }
    else if( messageType == MESSAGE_LOSE )
    {
    }
}
