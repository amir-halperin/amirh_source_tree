#pragma once

#include <fstream>

namespace fsm {

class Serializable
{
public:

  virtual ~Serializable() {}
  virtual void Save(std::ostream& os) {}
  virtual bool Load(std::istream& is) {return true;}
};

}; // namespace fsm