#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <fstream>
#include <string>
#include <string.h> // USES strcmp()

#include <fsm/Machine.h>
#include <fsm/State.h>
#include <fsm/MachineSerializer.h>
#include <fsm/StateFactory.h>

#include "AllStates.h"
#include "TransitionActions.h"
#include "TransitionActionsMock.h"

// definition of state machine with initial stStart state
typedef fsm::Machine<stStart, TransitionActions> TestMachine; 

// helper function used by SaveAndLoad test
void InitStateFactory(fsm::StateFactory* sf, TransitionActions& actions)
{
  sf->Insert(new stStart(actions));
  sf->Insert(new stA(actions));
  sf->Insert(new stAa(actions));
  sf->Insert(new stAab(actions));
  sf->Insert(new stAc(actions));
  sf->Insert(new stB(actions));
  sf->Insert(new stC(actions));
  sf->Insert(new stCStar(actions));
}
// helper function used by SaveAndLoad test
bool SaveMachineToFile(fsm::MachineSerializer& serializer, fsm::MachineBase& machine)
{
  std::fstream file("fsm.bin",std::ios::out | std::ios::binary);
  if(!file.is_open()) 
  {
    return false;
  }

  //f.open("fsm.bin");
  serializer.Save(file, machine);
  file.close();

  return true;
}
// helper function used by SaveAndLoad test
bool LoadMachineFromFile(fsm::MachineSerializer& serializer, fsm::MachineBase* machine)
{
  std::fstream file("fsm.bin",std::ios::in | std::ios::binary);
  if(!file.is_open()) 
  {
    return false;
  }

  bool isLoadSucceedded = serializer.Load(file, machine);

  file.close();

  return isLoadSucceedded;
}

// testing that the machine is saved in the middle of sequence 3
// restored and continue with events to complete sequence 3
TEST(fsm, SaveAndLoad)
{
  // create and set the mock
  TransitionActionsMock actions;
  //std::cout << "start SaveAndLoadTest\n";
  // initialized states factory to be used by MachineSerializer to create State according to State type
  fsm::StateFactory sf;
  // init factory should be done at first before serializer is used
  InitStateFactory(&sf, actions);
  // init serializer
  fsm::MachineSerializer ms(sf);

  // create a machine and move it into some statef sequence 3
  // restored and continue with events to complete sequ
  // testing that the machine is saved in the middle oence 3
  TestMachine* m = new TestMachine(actions, 11);

  evA a;
  evB b;
  evC c;

  // injecting ABC which is part of sequence 3 i.e. full sequence is ABC*1000
  m->ProcessEvent( a );
  m->ProcessEvent( b );
  m->ProcessEvent( c );

  // save the machine status
  SaveMachineToFile(ms, *m);
  // destroy the machine
  delete m;

  // reload machine
  // expect the machine to be in state stC* and guard == 1
  TestMachine restoredMachine(actions);
  // restore the state machine
  bool isLoadSucceeded = LoadMachineFromFile(ms, &restoredMachine);
  // expects that loading operation shall succeeded
  EXPECT_TRUE(isLoadSucceeded);
  // expect that Sequence2 action will be called after Machine recieves 1000 X evC events
  EXPECT_CALL(actions, Sequence2(testing::Eq(c))).Times(testing::AtLeast(1));
  // continue with the test only machine was restored successfully
  if(isLoadSucceeded)
  {
    // need to continue injecting evC 999 more time to receive sequence 2
    for(int i=0; i<999; i++)
    {
      restoredMachine.ProcessEvent( c );
    }
  }
}
// test that injecting events aabca triggers actions Sequence1
TEST(fsm, sequence1)
{
  TransitionActionsMock actions;
  fsm::Machine<stStart, TransitionActions> m(actions, 1);

  evA a;
  evB b;
  evC c;

  EXPECT_CALL(actions, Sequence1(testing::Eq(a))).Times(testing::AtLeast(1));

  // sequence 1
  m.ProcessEvent(a);
  m.ProcessEvent(a);
  m.ProcessEvent(b);
  m.ProcessEvent(c);
  m.ProcessEvent(a);
}
// test that injecting events ab and 1000 X c triggers actions Sequence2
TEST(fsm, sequence2)
{
  TransitionActionsMock actions;
  TestMachine m(actions, 1);

  evA a;
  evB b;
  evC c;

  EXPECT_CALL(actions, Sequence2(testing::Eq(c))).Times(testing::AtLeast(1));

  m.ProcessEvent(a);
  m.ProcessEvent(b);
  for(int i=0; i<1000; i++)
  {
    m.ProcessEvent(c);
  }
}
// test that injecting events acb and any additional event triggers actions Sequence3
TEST(fsm, sequence3)
{
  TransitionActionsMock actions;
  TestMachine m(actions, 1);

  evA a;
  evB b;
  evC c;

  EXPECT_CALL(actions, Sequence3(testing::Eq(b))).Times(testing::AtLeast(1));

  m.ProcessEvent(a);
  m.ProcessEvent(c);
  m.ProcessEvent(b);
  m.ProcessEvent(c);
}
// test State factory basic operation
TEST(fsm, StateFactoryTest)
{
  // create empty state factory
  fsm::StateFactory f;
  //
  TransitionActionsMock actions;
  // create state to be used as a prototype
  stStart* start = new stStart(actions);
  // init the factory with prototype state
  f.Insert(start);
  // create state by calling the factory with state's type
  fsm::StateBase* s = f.Create(start->Type());
  // verify created state is stStart
  EXPECT_EQ(strcmp(s->Type(), "stStart"), 0);
}

namespace boost 
{
    /*
    This file is a "null" implementation of tss cleanup; it's
    purpose is to to eliminate link errors in cases
    where it is known that tss cleanup is not needed.
    */

  void tss_cleanup_implemented(void) {}
}; // namespace boost
