#pragma once

#include <string>

namespace fsm {

// Abstract class that define state machine event.
// Designed to be used by Machine, State, and Transition classes.
// Must be derived into a specific type before use.
// Minimal implementation shall provide at least a unique type.
// Optionally, additional parameters can be added into the derived event class. 
// Such parameters if added, shall be used by specific States, and/or specific Transition classes.
class EventBase
{
public:

  virtual ~EventBase() {}
  // Type of event must be a unique string among all events in the system.
  // Good practice is to use the name of derive class name.
  virtual const std::string Type() const =0;

  bool operator==(const EventBase& other) const;

};

template<typename STATE_TYPE>
class Event : public EventBase
{
public:

  Event(std::string type) : mType(type) {}

  virtual const std::string Type() const {return mType.c_str();}

private:

  std::string mType;
};

}; // namespace fsm_fw