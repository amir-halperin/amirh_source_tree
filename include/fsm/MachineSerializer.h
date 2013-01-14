#pragma once

#include <fstream>

namespace fsm {

class MachineBase;
class StateFactory;

// Responsible to save serialized Machine objects.
// The MachineSerializer is friend of Machine class, so Machine class remain unaware
// to the serialization complexity.
class MachineSerializer
{
public:

  MachineSerializer(StateFactory& sf) : mStatefactory(sf) {}

  // save received Machine object into a given output stream
  void Save(std::ostream& os, MachineBase& m) const;
  // load received Machine object from a given input stream
  bool Load(std::istream& io, MachineBase* m) const;

private:

  // factory of State objects allow MachineSerializer to restore Machine's current state based on state's type
  const StateFactory& mStatefactory;
};

}; //namespace fsm