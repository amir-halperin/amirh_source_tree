#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <string.h> // USES strcmp()

#include <fsm/Machine.h>
#include <fsm/State.h>

#include "Events.h"
#include "ActionsMock.h"
#include "AdminDownState.h"


// definition of state machine with initial stStart state
typedef fsm::Machine<AdminDownState, TransitionActions> PortFsm;

TEST(fsmHTests, test1)
{
  ActionsMock actions;
  PortFsm m(actions);

  AdminUpEvent adminUp;
  AdminDownEvent adminDown;
  UpEvent up;
  DownEvent down;

  EXPECT_CALL(actions, Activated(testing::Eq(adminUp))).Times(testing::AtLeast(1));
  EXPECT_CALL(actions, PortChangeStateToUp(testing::Eq(up))).Times(testing::AtLeast(1));
  EXPECT_CALL(actions, DeActivated(testing::Eq(adminDown))).Times(testing::AtLeast(1));

  m.ProcessEvent(adminUp);
  m.ProcessEvent(up);
  m.ProcessEvent(adminDown);
}
