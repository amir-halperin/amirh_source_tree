#include "WorkingQUTHelper.h"

namespace wqut
{
  std::string STRING_ARG("this is the string argument");
  int INT_ARGUMENT = 234;
  int INT2_ARGUMENT = 987;
  double DOUBLE_ARGUMENT = 98.234;
  char CHAR_ARGUMENT = 't';
  int Foo::ctor_calls = 0;
  int Foo::dtor_calls = 0;
  int Foo::one_args_call = 0;
  int Foo::two_args_call = 0;
  int Foo::three_args_call = 0;
  int Foo::four_args_call = 0;
      
  int Bar::ctor_calls = 0;
  int Bar::dtor_calls = 0;
  int Bar::one_args_call = 0;
  int Bar::two_args_call = 0;
  int Bar::three_args_call = 0;
  int Bar::four_args_call = 0;
  int TestFunc1Counter = 0;
  int TestFunc2Counter = 0;
}
