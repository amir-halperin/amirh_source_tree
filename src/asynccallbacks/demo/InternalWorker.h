#pragma once

#include <gmock/gmock.h> // USES MOCK_METHOD1
#include <string>        // USES std::string
#include "ParameterType.h"

namespace asynccallbacks { namespace demo {

//class ParameterType;

/**
 * This class is used as a mock to ease the testing of ActiveClass that demonstrate Executer usage.
 * ActiveClass delegates execution of its internal private methods to its InternalWorker object. 
 * By that delegation it is possible to set call expectations on methods of this class in the unit-test
 * of the ActiveClass.
 */
class InternalWorker
{
public:

  MOCK_METHOD1(DoItOnPrimitive, void(int a));
  MOCK_METHOD1(DoItOnObject, void(std::string s));
  MOCK_METHOD2(DoItOnMultipleParameters, void(double d1, double d2));
  MOCK_METHOD1(DoItOnClassReference, void(ParameterType& param));
};

}} // namespace asynccallbacks::demo
