#pragma once

#include <fsm/Event.h>

class TransitionActions
{
public:

  virtual void Sequence1(const fsm::EventBase& e)=0;
  virtual void Sequence2(const fsm::EventBase& e)=0;
  virtual void Sequence3(const fsm::EventBase& e)=0;
};

#include <iostream>

class TransitionActionsImpl : public TransitionActions
{
public:

  virtual void Sequence1(const fsm::EventBase& e) {std::cout << "Sequeucen1\n";}
  virtual void Sequence2(const fsm::EventBase& e) {std::cout << "Sequeucen2\n";}
  virtual void Sequence3(const fsm::EventBase& e) {std::cout << "Sequeucen3:"<< e.Type() << "\n";}
};