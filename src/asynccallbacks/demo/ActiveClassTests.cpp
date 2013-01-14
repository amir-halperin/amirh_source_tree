#include "gtest/gtest.h"
#include <string>
#include "ActiveClass.h"
#include "ParameterType.h"
#include "InternalWorker.h"

using ::testing::Mock;
using ::testing::_;
using testing::InSequence;

using asynccallbacks::demo::InternalWorker;
using asynccallbacks::demo::ActiveClass;
using asynccallbacks::demo::ParameterType;

namespace { // all code in the unit test is local to this file 
    
TEST(ActiveClass, CallWithPrimitiveType)
{
  InternalWorker worker;
  ActiveClass myActiveObject(worker);
      
  EXPECT_CALL(worker, DoItOnPrimitive(123));
  myActiveObject.DoItOnPrimitive(123);
}

TEST(ActiveClass, CallWithObjectByVal)
{
  InternalWorker worker;
  ActiveClass myActiveObject(worker);

  EXPECT_CALL(worker, DoItOnObject("My name is Mr. Kashtan"));
  myActiveObject.DoItOnObject("My name is Mr. Kashtan");
}

TEST(ActiveClass, CallWithObjectByRef)
{
  InternalWorker worker;
  ActiveClass myActiveObject(worker);
  
  ParameterType p1;
  p1.mNumber = 300870;
  EXPECT_CALL(worker, DoItOnClassReference(p1));
  
  ParameterType myParam;
  myParam.mNumber = 300870;
  myActiveObject.DoItOnClassReference(myParam);
}

TEST(ActiveClass, CallWithObjectByPointer)
{
  InternalWorker worker;
  ActiveClass myActiveObject(worker);
  
  ParameterType p1;
  p1.mNumber = 300870;
  EXPECT_CALL(worker, DoItOnClassReference(p1));
  
  ParameterType* myParam = new ParameterType;
  myParam->mNumber = 300870;
  myActiveObject.DoItOnClassPointer(myParam);
  delete myParam;
}

TEST(ActiveClass, CallWithMultipleParameters)
{
  InternalWorker worker;
  ActiveClass myActiveObject(worker);
      
  EXPECT_CALL(worker, DoItOnMultipleParameters(432, 5679));
  myActiveObject.DoItOnMultipleParameters(432, 5679); 
}

} // end of local namespace

