//-----------------------------------------------------------------------------
// Daniel White
// 10/25/08
// 
//
// Description:
//	The MessageQueue decides which messages to display
//-----------------------------------------------------------------------------


#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Message.h"
#include "vec3.h"

using namespace::std;

enum Messages {
    MESSAGE_TIME_ORB = 99,
    MESSAGE_JUMP_ORB,
    MESSAGE_LOSE,
    MESSAGE_WIN
};

class MessageQueue
{
public:
    static MessageQueue* getInstance();
    void showMessage(unsigned int messageType, Vec3f position);

protected:   

private:
    MessageQueue();
    static MessageQueue* inst;
};

#endif
