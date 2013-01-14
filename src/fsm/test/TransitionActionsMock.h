#pragma once

#include <gmock/gmock.h>
#include <string>
#include "TransitionActions.h"

class TransitionActionsMock : public TransitionActions
{
public:

  MOCK_METHOD1(Sequence1, void(const fsm::EventBase&));
  MOCK_METHOD1(Sequence2, void(const fsm::EventBase&));
  MOCK_METHOD1(Sequence3, void(const fsm::EventBase&));
};