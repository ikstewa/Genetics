#include "Message.h"

int Message::decrementTime(int t)
{
   duration -= t;
   return duration;
}
