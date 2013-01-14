#ifndef MESSAGE_QUEUE_IMPLE_HPP
#define MESSAGE_QUEUE_IMPLE_HPP
// Note that in this implementation like in all the other implementation we would no operation that would be done
// so calling any function here would have no effect
// note that this is useful for unit tests where we would not like to test in any way the effect of calling this

#include <queue>    // to place messages inside it
#include <string>   // we would be using string as the data in the queue
#include <assert.h>
#include <algorithm>  // copy

namespace osal
{

namespace MessageQueue
{
  
struct Id
{
  std::queue<std::string> Data;
};

namespace
{
  
  Id* GetMessageQueueId()
  {
    return new Id;
  }
  
} // end of local namespace


void RegisterAtExit(at_error_fun)
{ 
}

Id* Create(unsigned int, unsigned int)
{
  return GetMessageQueueId();
}

void Send(Id* id, const char* data, unsigned int len, bool)
{
  assert(id);
  id->Data.push(std::string(data, len));
}

bool TrySend(Id* id, const char* data, unsigned int len, bool)
{
  assert(id);
  id->Data.push(std::string(data, len));
  return true;
}

bool TimedSend(Id* id, const char* data, unsigned int len, 
               milliseconds_t , bool )
{
  assert(id);
  id->Data.push(std::string(data, len));
  return true;
}

int Receive(Id* id, char* data, unsigned int buffSize)
{
  
  assert(id);
  if (!id->Data.empty())
  {
    std::string& s(id->Data.front());
    if (s.size() > buffSize)
    {
      s.resize(buffSize); // remove any elements not in range
    }
    std::copy(s.begin(), s.end(), data);
    buffSize = s.size();
    id->Data.pop();
  }
  else
  {
    buffSize = -1;
  }
  return buffSize;
}

int TryReceive(Id* id, char* data, unsigned int buffSize)
{
  assert(id);
  if (!id->Data.empty())
  {
    std::string& s(id->Data.front());
    if (s.size() > buffSize)
    {
      s.resize(buffSize); // remove any elements not in range
    }
    std::copy(s.begin(), s.end(), data);
    buffSize = s.size();
    id->Data.pop();
  }
  else
  {
    buffSize = -1;
  }
  return buffSize;
}

int TimedReceive(Id* id, char* data, unsigned int buffSize, milliseconds_t )
{
  assert(id);
  if (!id->Data.empty())
  {
    std::string& s(id->Data.front());
    if (s.size() > buffSize)
    {
      s.resize(buffSize); // remove any elements not in range
    }
    std::copy(s.begin(), s.end(), data);
    buffSize = s.size();
    id->Data.pop();
  }
  else
  {
    buffSize = -1;
  }
  return buffSize;
}

int CurrentCount(Id* id)
{
  assert(id);
  return id->Data.size();
}

void Delete(Id*& id)
{
 delete id;
 id = 0;
}

} // end of namespace MessageQueue
} // end of namespace osal

#else
# error "you must not include this file inside another header file"
#endif //MESSAGE_QUEUE_IMPLE_HPPP
