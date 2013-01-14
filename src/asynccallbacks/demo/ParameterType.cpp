#include "ParameterType.h"

namespace asynccallbacks { namespace demo {

ParameterType::ParameterType() : mNumber(0) {}
  
bool ParameterType::operator==(const ParameterType& other)
{
  if(mNumber == other.mNumber) 
  {
    return true;
  }
  
return false;
}

}} // namespace asynccallbacks::demo
