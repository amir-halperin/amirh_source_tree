#pragma once

#include <map>
#include <string>

namespace fsm {

class StateBase;

// Responsible to create State objects according to given State's type.
// Used by MachineSerializer during Load procedure.
class StateFactory
{
public:
  // Interface for initializing the factory.
  // Supposed to be called once per State type. Duplicated state prototypes are rejected gracefully.
  void Insert(StateBase* prototype);
  // Interface to create State object according to State's type.
  // Return 0 if type is not found.
  StateBase* Create(std::string type) const;

private:

  typedef std::map<std::string, StateBase*> StringToStateMapType;
  typedef std::map<std::string, StateBase*>::const_iterator StringToStateMapConstInteratorType;
  typedef std::pair<std::string, StateBase*> StringStatePairType;

  StringToStateMapType mStatePrototypes;
};

}; // namespace fsm