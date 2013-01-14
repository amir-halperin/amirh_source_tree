#pragma once

#include <string>
#include <asynccallbacks/Executer.h>

namespace asynccallbacks { namespace demo {
  
// forward declaration
class ParameterType;  // to demonstrate how ActiveClass accept object parameter 
class InternalWorker; // gmock object for testing internal expected calls (see ExecuterDemoUT) 

/**
 * ActiveClass demonstrate how Executer infrastructure class should be used.
 * ActiveClass is a class that executes client calls in the context of its internal thread.
 * Public methods of this class delegates received parameters into an internal Executer object.
 * The Executer do the following:
 *  - packs received parameters and creates some sort of function object i.e. functor
 *  - insert the functor into an internal queue
 *  - Extract & activate functors received from the queue in the context of Executer's internal thread
 * The ActiveClass is responsible for:
 *  - Provide proper functional interface i.e. regular public methods
 *  - Implement internal private methods to perform the actual required job
 *  - Correlate between public and private methods using Executer
 *  - Create and initialized internal Executer object
 *  
 *  Executer can support any kind of input parameter.
 *  Limitation: Executer doesn't support return values from any kind.
 */
class ActiveClass
{
public:
  
  /**
   * ctor of the class is responsible to create and initiate internal executer 
   * before any public interface is called.
   * @param w is a mock object that is used to verify that call delegation is actually works. 
   */
  ActiveClass(InternalWorker& w);
  // dtor of the call is responsible to destroy the internal executer
  ~ActiveClass();
  /**
   * Interface method of the class that delegates the actual job to be done in the context of an internal executer.
   * Demonstrate delegation of primitive parameter type.
   * @param a primitive parameter type
   */
  void DoItOnPrimitive(int a);
  /**
   * Demonstrate delegation of an object parameter by value.
   * @param s string as an object parameter.
   */
  void DoItOnObject(std::string s);
  /**
   * Demonstrate delegation of multiple parameters
   * @param d1 first primitive
   * @param d2 second primitive
   */
  void DoItOnMultipleParameters(double d1, double d2);
  /**
   * Demonstrate delegation of referenced object.
   * No problem to accept reference to objects.
   * The infrastructure behind the scenes copies the object, thus the internal method 
   * that perform the job received local object instance.
   * @param param an example for reference object type.
   */
  void DoItOnClassReference(ParameterType& param);
  /**
   * Demonstrate delegation of pointer to object.
   * When a parameter is transfer as a pointer the executer shall received it by value
   * to make sure the parameter object is copied.
   * Note that the internal method that doing the job i.e. f5 in this example, can accept
   * the parameter by value or by reference.
   * @param paramP an example for a pointer to object.
   */
  void DoItOnClassPointer(ParameterType* paramP);
  
private:
  
  // internal methods that perform the class's actual job in the context of the internal executer.
  // each method is correlated to a single public interface method.
  void DoItOnPrimitiveInternal(int a);
  void DoItOnObjectInternal(std::string s);
  void DoItOnMultipleParametersInternal(double d1, double d2);
  void DoItOnClassReferenceInternal(ParameterType& param);
  
  // an internal executer that executes internal jobs in its internal context
  // user class (in this example the ActiveClass) responsible to define executer thread's name, queue size, and priority.
  asynccallbacks::Executer<ActiveClass> mExecuter;
  // This class is used as a mock to ease the testing of ActiveClass that demonstrate Executer usage.
  // ActiveClass delegates execution of its internal private methods to its InternalWorker object. 
  // By that delegation it is possible to set call expectations on methods of this class in the unit-test
  // of the ActiveClass.
  InternalWorker& mInternalWorker;
};

}} // namespace asynccallbacks::demo

