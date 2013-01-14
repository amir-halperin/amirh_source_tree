#pragma once

namespace asynccallbacks { namespace demo {
  
class ParameterType
{
public:
  
  ParameterType();
  
  bool operator==(const ParameterType& other);
  
  int mNumber;
};

}} // namespace asynccallbacks::demo
