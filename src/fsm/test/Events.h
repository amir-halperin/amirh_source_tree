#pragma once

#include <fsm/Event.h>
#include <string>

class evA : public fsm::Event<evA>
{
public:
  evA() : Event("evA") {}
};

class evB: public fsm::Event<evB>
{
public:
  evB() : Event("evB") {}
};

class evC: public fsm::Event<evC>
{
public:
  evC() : Event("evC") {}
};
