#include "ActiveClass.h"
#include <iostream>
#include "ParameterType.h"
#include "InternalWorker.h"
#include <osal/Thread.h> // USES osal:thread::PRIORITY_20

namespace asynccallbacks { namespace demo {
  
const int QUEUE_SIZE = 10;

////////////////////////////////////////////////////////
// PUBLIC
////////////////////////////////////////////////////////

ActiveClass::ActiveClass(InternalWorker& w)
  : mExecuter(this, QUEUE_SIZE), mInternalWorker(w)
{
  // activate internal async Executer
  mExecuter.Start("ActiveClassTask", osal::Thread::PRIORITY_20);
}

ActiveClass::~ActiveClass()
{
  mExecuter.Stop();
}

void ActiveClass::DoItOnPrimitive(int a)
{
  mExecuter.Call(&ActiveClass::DoItOnPrimitiveInternal, a);
}
  
void ActiveClass::DoItOnObject(std::string s) 
{
  mExecuter.Call(&ActiveClass::DoItOnObjectInternal, s);
}

void ActiveClass::DoItOnMultipleParameters(double d1, double d2)
{
  mExecuter.Call(&ActiveClass::DoItOnMultipleParametersInternal, d1, d2);
}

void ActiveClass::DoItOnClassReference(ParameterType& param)
{
  // No problem to accept reference to objects.
  // The infrastructure behind the scenes copies the object, thus the internal method 
  // that perform the job received local object instance. 
  mExecuter.Call(&ActiveClass::DoItOnClassReferenceInternal, param);
}

void ActiveClass::DoItOnClassPointer(ParameterType* paramP)
{
  // When a parameter is transfered as a pointer the executer shall received it by value
  // to make sure the parameter object is copied.
  // Note that the internal method that doing the job i.e. f5 in this example, can accept
  // the parameter by value or by reference. 
  // In this case the job is delegated to the same internal method as the DoItOnClassReference was delegated to.
  mExecuter.Call(&ActiveClass::DoItOnClassReferenceInternal, *paramP);
}

////////////////////////////////////////////////////////
// PRIVAT
////////////////////////////////////////////////////////

void ActiveClass::DoItOnPrimitiveInternal(int a)
{
  mInternalWorker.DoItOnPrimitive(a);
}

void ActiveClass::DoItOnObjectInternal(std::string s)
{
  mInternalWorker.DoItOnObject(s);
}

void ActiveClass::DoItOnMultipleParametersInternal(double d1, double d2)
{
  mInternalWorker.DoItOnMultipleParameters(d1, d2);
}

void ActiveClass::DoItOnClassReferenceInternal(ParameterType& param)
{
  mInternalWorker.DoItOnClassReference(param);
}
  
}} // namespace asynccallbacks::demo
