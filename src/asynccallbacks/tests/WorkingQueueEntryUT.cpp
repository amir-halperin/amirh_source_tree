#include "WorkingQUTHelper.h"
#include "gtest/gtest.h"
#include "asynccallbacks/details/WorkingQueueEntry.h"
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
#include <algorithm>
#include <iostream>

namespace
{   // test are always local to the file in which the tests are writing

  
    using namespace wqut;
    using namespace asynccallbacks;
    
    void RunWQEntry(const WorkingQueueEntry& wqe)
    {
      wqe();
    }
    
    ////////////////////////////////////////////////////////////////
    
    class TestWorkingQ : public ::testing::Test
    {
    public:
      TestWorkingQ()
      {
        
      }
      
    protected:
      virtual void SetUp()
      {
        TestFunc1Counter = 0;
        TestFunc2Counter = 0;
        Foo::ctor_calls = 0;
        Foo::dtor_calls = 0;
        Foo::one_args_call = 0;
        Foo::two_args_call = 0;
        Foo::three_args_call = 0;
        Foo::four_args_call = 0;
        Bar::ctor_calls = 0;
        Bar::dtor_calls = 0;
        Bar::one_args_call = 0;
        Bar::two_args_call = 0;
        Bar::three_args_call = 0;
        Bar::four_args_call = 0;
        
      }
      
    };
    
    
    TEST_F(TestWorkingQ, FreeFuncTest)
    {
      EXPECT_EQ(0, TestFunc1Counter);
      EXPECT_EQ(0, TestFunc2Counter);
      std::auto_ptr<WorkingQueueEntry> re1 = MakeWorkingQueueEntry(TestFunc1, INT_ARGUMENT, STRING_ARG, DOUBLE_ARGUMENT, CHAR_ARGUMENT, INT2_ARGUMENT); 
      std::auto_ptr<WorkingQueueEntry> re2 = MakeWorkingQueueEntry(TestFunc2, STRING_ARG, INT_ARGUMENT);
      // make sure that the functions were not called yet!
      EXPECT_EQ(0, TestFunc1Counter);
      EXPECT_EQ(0, TestFunc2Counter);
      RunWQEntry(*re1);
      EXPECT_EQ(1, TestFunc1Counter);
      EXPECT_EQ(0, TestFunc2Counter);
      RunWQEntry(*re1);
      EXPECT_EQ(2, TestFunc1Counter);
      EXPECT_EQ(0, TestFunc2Counter);
      RunWQEntry(*re2);
      EXPECT_EQ(2, TestFunc1Counter);
      EXPECT_EQ(1, TestFunc2Counter);
    }
    
    TEST_F(TestWorkingQ, MemFunctionTest)
    {
       EXPECT_EQ(0, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       
       boost::ptr_vector<WorkingQueueEntry> entries;
       Bar bar;
       entries.push_back(MakeWorkingQueueEntry(&Bar::OneArg, &bar, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::TwoArg, &bar, DOUBLE_ARGUMENT, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::FourArg, &bar, INT_ARGUMENT, &STRING_ARG, DOUBLE_ARGUMENT, INT2_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::ThreeArg, &bar, DOUBLE_ARGUMENT, STRING_ARG, INT_ARGUMENT)); 
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       unsigned int i = 0;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(1, Bar::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(1, Bar::three_args_call);
       EXPECT_EQ(1, Bar::four_args_call);
    }
    
    TEST_F(TestWorkingQ, MixMemFunc)
    {
      // test with more than one class register for execution
       EXPECT_EQ(0, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       EXPECT_EQ(0, Foo::ctor_calls);
       EXPECT_EQ(0, Foo::dtor_calls );
       EXPECT_EQ(0, Foo::one_args_call);
       EXPECT_EQ(0, Foo::two_args_call);
       EXPECT_EQ(0, Foo::three_args_call);
       EXPECT_EQ(0, Foo::four_args_call);
       
       boost::ptr_vector<WorkingQueueEntry> entries;
       Foo foo;
       Bar bar;
       entries.push_back(MakeWorkingQueueEntry(&Foo::OneArg, &foo, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::TwoArg, &bar, DOUBLE_ARGUMENT, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::FourArg, &bar, INT_ARGUMENT, &STRING_ARG, DOUBLE_ARGUMENT, INT2_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Foo::ThreeArg, &foo, DOUBLE_ARGUMENT, STRING_ARG, INT_ARGUMENT)); 
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       EXPECT_EQ(1, Foo::ctor_calls);
       EXPECT_EQ(0, Foo::dtor_calls );
       EXPECT_EQ(0, Foo::one_args_call);
       EXPECT_EQ(0, Foo::two_args_call);
       EXPECT_EQ(0, Foo::three_args_call);
       EXPECT_EQ(0, Foo::four_args_call);
       
       unsigned int i = 0;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       
       EXPECT_EQ(1, Foo::ctor_calls);
       EXPECT_EQ(0, Foo::dtor_calls );
       EXPECT_EQ(1, Foo::one_args_call);
       EXPECT_EQ(0, Foo::two_args_call);
       EXPECT_EQ(0, Foo::three_args_call);
       EXPECT_EQ(0, Foo::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       
       EXPECT_EQ(1, Foo::ctor_calls);
       EXPECT_EQ(0, Foo::dtor_calls );
       EXPECT_EQ(1, Foo::one_args_call);
       EXPECT_EQ(0, Foo::two_args_call);
       EXPECT_EQ(0, Foo::three_args_call);
       EXPECT_EQ(0, Foo::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(1, Bar::four_args_call);
       
       EXPECT_EQ(1, Foo::ctor_calls);
       EXPECT_EQ(0, Foo::dtor_calls );
       EXPECT_EQ(1, Foo::one_args_call);
       EXPECT_EQ(0, Foo::two_args_call);
       EXPECT_EQ(0, Foo::three_args_call);
       EXPECT_EQ(0, Foo::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(1, Bar::four_args_call);
       
       EXPECT_EQ(1, Foo::ctor_calls);
       EXPECT_EQ(0, Foo::dtor_calls );
       EXPECT_EQ(1, Foo::one_args_call);
       EXPECT_EQ(0, Foo::two_args_call);
       EXPECT_EQ(1, Foo::three_args_call);
       EXPECT_EQ(0, Foo::four_args_call);
    }
    
    TEST_F(TestWorkingQ, InvalidUse)
    {
      // in this test we would like to make sure that if we try to
      // register the object itself (not passing reference or pointer)
      // we would have the ctor and dtor called again and again - which
      // means that we would not have any object to call on when we would try
      // using this infrastructures
       EXPECT_EQ(0, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       
       boost::ptr_vector<WorkingQueueEntry> entries;
       Bar bar;
       entries.push_back(MakeWorkingQueueEntry(&Bar::OneArg, bar, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::TwoArg, bar, DOUBLE_ARGUMENT, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::FourArg, bar, INT_ARGUMENT, &STRING_ARG, DOUBLE_ARGUMENT, INT2_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::ThreeArg, bar, DOUBLE_ARGUMENT, STRING_ARG, INT_ARGUMENT)); 
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_LE(4, Bar::dtor_calls );  // note that we have too many calls to destruct the object!!!
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
      
    }
    
    TEST_F(TestWorkingQ, UsingRefToObject)
    {
      
       EXPECT_EQ(0, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       
       boost::ptr_vector<WorkingQueueEntry> entries;
       Bar bar;
       entries.push_back(MakeWorkingQueueEntry(&Bar::OneArg, boost::cref(bar), INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::TwoArg, boost::ref(bar), DOUBLE_ARGUMENT, INT_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::FourArg, boost::ref(bar), INT_ARGUMENT, &STRING_ARG, DOUBLE_ARGUMENT, INT2_ARGUMENT));
       entries.push_back(MakeWorkingQueueEntry(&Bar::ThreeArg, boost::cref(bar), DOUBLE_ARGUMENT, STRING_ARG, INT_ARGUMENT)); 
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(0, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       unsigned int i = 0;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(0, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(0, Bar::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(0, Bar::three_args_call);
       EXPECT_EQ(1, Bar::four_args_call);
       i++;
       RunWQEntry(entries[i]);
       EXPECT_EQ(1, Bar::ctor_calls);
       EXPECT_EQ(0, Bar::dtor_calls );
       EXPECT_EQ(1, Bar::one_args_call);
       EXPECT_EQ(1, Bar::two_args_call);
       EXPECT_EQ(1, Bar::three_args_call);
       EXPECT_EQ(1, Bar::four_args_call);
    }
}
