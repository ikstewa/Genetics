//-----------------------------------------------------------------------------
// Daniel White
// 10/25/08
// 
//
// Description:
//	This is a message on the screen.
//-----------------------------------------------------------------------------


#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <string>

#define MESSAGE_PIORITY_LOW    0
#define MESSAGE_PIORITY_MEDIUM 1
#define MESSAGE_PIORITY_HIGH   2

class Message
{
public:
    Message() {text="";duration=0;priority=MESSAGE_PIORITY_LOW;}
    Message(std::string str, int d, int pri) {text = str; duration = d*1000; priority = pri;} //note duration from exterior point of view is seconds
    ~Message() {}
    int decrementTime(int t); //subtract duration - t, and return new duration
    std::string getString() {return text;}
    int getPriority() {return priority;}
protected:   

private:
    std::string text;
    int priority;
    int duration;  //in milliseconds
};

#endif
