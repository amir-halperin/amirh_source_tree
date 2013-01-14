#pragma once

#include <fsm/Event.h>

namespace fsm {

class EventsProcessor
{
public:

  virtual void ProcessEvent(const EventBase& e)=0;
};

}; // namespace fsm