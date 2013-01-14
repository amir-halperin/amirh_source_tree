#pragma once

#include "gtest/gtest.h"
#include <string>


namespace wqut
{
  extern std::string STRING_ARG;
  extern int INT_ARGUMENT;
  extern int INT2_ARGUMENT;
  extern double DOUBLE_ARGUMENT;
  extern char CHAR_ARGUMENT;
  
  class Foo
  {
  public:
    Foo()
    {
      ctor_calls++;
    }
    
    ~Foo()
    {
      dtor_calls++;
    }
    
    void OneArg(int i) const
    {
      EXPECT_EQ(i, INT_ARGUMENT);
      one_args_call++;
    }
    
    void TwoArg(double d, int i)
    {
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      two_args_call++;
    }
    
    void ThreeArg(double d, const std::string& s, int i) const
    {
      three_args_call++;
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      EXPECT_EQ(s, STRING_ARG);
    }
    
    void FourArg(int i, std::string* s, double d, int i2)
    {
      four_args_call++;
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      EXPECT_EQ(*s, STRING_ARG);
      EXPECT_EQ(INT2_ARGUMENT, i2);
    }
    
    static int ctor_calls;
    static int dtor_calls;
    static int one_args_call;
    static int two_args_call;
    static int three_args_call;
    static int four_args_call;
  };
    
  
  class Bar
  {
  public:
    Bar()
    {
      ctor_calls++;
    }
    
    ~Bar()
    {
      dtor_calls++;
    }
    
    void OneArg(int i) const
    {
      EXPECT_EQ(i, INT_ARGUMENT);
      one_args_call++;
    }
    
    void TwoArg(double d, int i)
    {
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      two_args_call++;
    }
    
    void ThreeArg(double d, const std::string& s, int i) const
    {
      three_args_call++;
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      EXPECT_EQ(s, STRING_ARG);
    }
    
    void FourArg(int i, std::string* s, double d, int i2)
    {
      four_args_call++;
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      EXPECT_EQ(*s, STRING_ARG);
      EXPECT_EQ(INT2_ARGUMENT, i2);
    }
    static int ctor_calls;
    static int dtor_calls;
    static int one_args_call;
    static int two_args_call;
    static int three_args_call;
    static int four_args_call;
  };
  
    
    
    extern int TestFunc1Counter;
    extern int TestFunc2Counter;
    
    inline int TestFunc1(int i, std::string& s, double d, char c, int i2)
    {
      EXPECT_EQ(d, DOUBLE_ARGUMENT);
      EXPECT_EQ(i, INT_ARGUMENT);
      EXPECT_EQ(s, STRING_ARG);
      EXPECT_EQ(INT2_ARGUMENT, i2);
      EXPECT_EQ(CHAR_ARGUMENT,c);
      
      TestFunc1Counter++;
      return TestFunc1Counter;
    }
    
    inline int TestFunc2(std::string s, int i)
    {
      EXPECT_EQ(i, INT_ARGUMENT);
      EXPECT_EQ(s, STRING_ARG);
      TestFunc2Counter++;
      return TestFunc2Counter;
    }
} // end of wqut
