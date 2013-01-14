#pragma once

#include <gmock/gmock.h>
#include "TransitionActions.h"
#include <fsm/Event.h>

class ActionsMock : public TransitionActions
{
public:

  MOCK_METHOD1( Activated, void (const fsm::EventBase& e) );
  MOCK_METHOD1( DeActivated, void (const fsm::EventBase& e) );
  MOCK_METHOD1( PortChangeStateToUp, void (const fsm::EventBase& e) );
  MOCK_METHOD1( PortChangeStateToDown, void (const fsm::EventBase& e) );
};
