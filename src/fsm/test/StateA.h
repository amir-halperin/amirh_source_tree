#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"

class TransitionActions;

class trAToAa : public fsm::Transition<trAToAa>
{
public:
  trAToAa (TransitionActions& actions) : Transition("trAToAa"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class trAToB : public fsm::Transition<trAToB>
{
public:
  trAToB(TransitionActions& actions) : Transition("trAToB"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class trAToAc : public fsm::Transition<trAToAc>
{
public:
  trAToAc(TransitionActions& actions) : Transition("trAToAc"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class stA : public fsm::State<stA>
{
public:

  stA(TransitionActions& actions) : State("stA"), mActions(actions)
  {
    CreateTransition(evA().Type(), new trAToAa(mActions));
    CreateTransition(evB().Type(), new trAToB(mActions));
    CreateTransition(evC().Type(), new trAToAc(mActions));
  }

private:

  TransitionActions& mActions;
};