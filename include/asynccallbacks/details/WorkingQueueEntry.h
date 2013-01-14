#pragma once
/**
 * @brief this the interface to a callback frame work
 * 
 * This file contain a @class WorkingQueueEntry and collection
 * of functions that can be used to register work into queue
 * and execute them at later stage. note that the return value
 * cannot be accessed
 * any function can be registered using this frame work
 * for example:
 *  given we have the class 
 *  class X
 *  {
 *  public:
 *    void foo(int, double, int, int) const
 *    {
 *      // do something here
 *    }
 *    
 *    int bar(double, double)
 *    {
 *      // something here
 *    }
 *    
 *    std::string& baz(std::string*, const std::string&, int, int, unsigned int) const
 *    {
 *      // what ever this does
 *    }
 *    // and the reset of the object is here
 *  }; 
 *  
 *  and the global functions 
 *  void global_foo()
 *  {
 *  }
 *  
 *  int global_bar(int, int, int, int)
 *  {
 *  }
 *  
 *  double global_baz(double, double, int, int, int)
 *  {
 *  }
 *  
 *  we can register all of those functions with the following calls
 *  
 *  boost::ptr_vector<WorkingQueueEntry> callbackList; --> note that this can be any collection!
 *  X x;  --> we need the instance of the class before registering!
 *  std::string someString("this is a string");
 *  callbackList.push_back(MakeWorkingQueueEntry(&X::foo, &x, 10, 12.2, 33, 4413)); --> note that we must pass either pointer to object
 *  callbackList.push_back(MakeWorkingQueueEntry(&X::bar, boost::ref(x), 99.4, 234.3)); --> or reference to it but not pass by value!!!!!
 *  callbackList.push_back(MakeWorkingQueueEntry(&X::baz, boost::cref(x), &someString, std::string("another string"), 90, 442, 234u);
 *  callbackList.push_back(MakeWorkingQueueEntry(global_foo); --> in case of global function need to pass the function is first param
 *  callbackList.push_back(MakeWorkingQueueEntry(global_bar, 19, 56, 13, 44); --> and pass parameters after it
 *  callbackList.push_back(MakeWorkingQueueEntry(global_baz, 1.345, 56.6667, 13, 44, 67);
 *  
 *  once the registeration is done we can let them work uniformaly
 *  
 *  void execute_function(const WorkingQueueEntry& exec)
 *  {
 *    exec();
 *  }
 *  
 *  std::for_each(callbackList.begin(), callbackList.end(), execute_function);
 *  
 */

#include <boost/function.hpp>   // boost::function is the holder of the "excutable"
#include <boost/bind.hpp>       // to bind functions and paramters
#include <boost/type_traits/remove_pointer.hpp> // so that we can test for valid parameters
#include <boost/type_traits/is_function.hpp>    //so that we can test for valid parameters 
#include <boost/type_traits/is_member_function_pointer.hpp> // so that we can test for valid parameters
#include <boost/type_traits/is_class.hpp>       // so that we can test for valid parameters
#include <boost/static_assert.hpp>              // so that we can test for valid parameters
#include <boost/type_traits/is_same.hpp>        // so that we can test for valid parameters
#include <boost/mpl/if.hpp>                     // so that we can test for valid parameters

namespace asynccallbacks
{
/**
 * @class WorkingQueueEntry
 * @brief the "interface to any callback that is registered
 * 
 * Use this object as the return value in which we would register functions 
 * and their variables to be executed later. The execution of the registered
 * entity is done through the call to operator () as nullary function (no
 * parameters are needed). 
 */
class WorkingQueueEntry
{
public:
  typedef boost::function<void()> function_type;
  
  /**
   * Use this constructor to register entity to be executed later through member operator ()
   * @param ft the function type that would be saved to this object
   */
  WorkingQueueEntry(function_type ft) : func(ft)
  {
  }
  
  /**
   * default constructor - note that if this is the only one that
   * called then nothing would happen
   */
  WorkingQueueEntry()
  {
  }
  
  /**
   * This operator would execute the registered entity as many times with
   * the same parameters that were passed to it when created
   */
  void operator () () const
  {
    func();
  }
  
private:
  function_type func;
};

namespace Private
{

struct MemFuncBinder
{
  template<typename F, typename C> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst)
  {
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst))));
  }
  
  template<typename F, typename C, typename A> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst, A a)
  {
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst, a)))); 
  }
  
  template<typename F, typename C, typename A, typename A2> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst, A a,  A2 a2)
  {   
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst, a, a2)))); 
  }
  
  
  template<typename F, typename C, typename A, typename A2, typename A3> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst, A a, A2 a2, A3 a3)
  {
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst, a, a2, a3)))); 
  }
  
  
  template<typename F, typename C, typename A, typename A2, typename A3, typename A4> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst, A a, A2 a2, A3 a3, A4 a4)
  {
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst, a, a2, a3, a4))));
  }
  
  template<typename F, typename C, typename A, typename A2, 
           typename A3, typename A4, typename A5> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst, A a, A2 a2, A3 a3, A4 a4, A5 a5)
  {
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst, a, a2, a3, a4, a5))));
  }
  
  template<typename F, typename C, typename A, typename A2, 
           typename A3, typename A4, typename A5, typename A6> static inline
  std::auto_ptr<WorkingQueueEntry> make(F mem_f, C inst, A a, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
  {
    return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(mem_f, inst, a, a2, a3, a4, a5, a6))));
  }
};

struct FreeFunctionBinder
{
  template<typename F, typename C> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst)
  {
    return MemFuncBinder::make(f, inst); 
  }
  
  template<typename F, typename C, typename A> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst, A a)
  {
    return MemFuncBinder::make(f, inst, a); 
  }
  
  template<typename F, typename C, typename A, typename A2> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst, A a,  A2 a2)
  {
    return MemFuncBinder::make(f, inst,a, a2); 
  }
  
  template<typename F, typename C, typename A, typename A2, typename A3> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst, A a, A2 a2, A3 a3)
  {
    return MemFuncBinder::make(f, inst,a, a2, a3); 
  }
  
  template<typename F, typename C, typename A, typename A2, typename A3, typename A4> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst, A a, A2 a2, A3 a3, A4 a4)
  {
    return MemFuncBinder::make(f, inst,a, a2, a3, a4); 
  }
  
  template<typename F, typename C, typename A, typename A2, typename A3, typename A4, typename A5> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst, A a, A2 a2, A3 a3, A4 a4, A5 a5)
  {
    return MemFuncBinder::make(f, inst,a, a2, a3, a4, a5); 
  }
  
  template<typename F, typename C, typename A, typename A2, typename A3, 
           typename A4, typename A5, typename A6> static inline
  std::auto_ptr<WorkingQueueEntry> make(F f, C inst, A a, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
  {
    return MemFuncBinder::make(f, inst,a, a2, a3, a4, a5, a6); 
  }
};

struct ErrorInvalidWorkingQueueEntry;

#define CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(Func, Class) \
          typedef typename boost::mpl::if_c<boost::is_function<typename boost::remove_pointer<F>::type>::value, \
                   Private::FreeFunctionBinder, \
                   typename boost::mpl::if_c<boost::is_member_function_pointer<F>::value && \
                                 boost::is_class<typename boost::remove_pointer<C>::type>::value, \
                            Private::MemFuncBinder, \
                              Private::ErrorInvalidWorkingQueueEntry>::type \
              >::type binder_type

} // end of Private namespace


/**
 * collection of functions that access as the first parameters a callable entity - either global function or
 * member function pointer and optionaly a list of parameters that must match the list of parameters that the
 * function that was register is accepting. note that if the registered function is member function
 * the second parameter must be a pointer to the object that the member function is operate on or reference to it
 * you cannot pass the object by value!
 */
template<typename F> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F f)
{
  BOOST_STATIC_ASSERT(boost::is_function<typename boost::remove_pointer<F>::type>::value);
  return std::auto_ptr<WorkingQueueEntry>(new WorkingQueueEntry(WorkingQueueEntry::function_type(boost::bind(f)))); 
}


template<typename F, typename C> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
  
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
  return binder_type::make(mem_f, inst);        
}

template<typename F, typename C, typename A> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst, A a)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
              
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
    return binder_type::make(mem_f, inst, a); 
}

template<typename F, typename C, typename A, typename A2> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst, A a,  A2 a2)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
              
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
    return binder_type::make(mem_f, inst, a, a2); 
}

template<typename F, typename C, typename A, typename A2, typename A3> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst, A a, A2 a2, A3 a3)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
    
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
    return binder_type::make(mem_f, inst, a, a2, a3); 
}

template<typename F, typename C, typename A, typename A2, typename A3, typename A4> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst, A a, A2 a2, A3 a3, A4 a4)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
  
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
    return binder_type::make(mem_f, inst, a, a2, a3, a4); 
}

template<typename F, typename C, typename A, typename A2, 
         typename A3, typename A4, typename A5> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst, A a, A2 a2, A3 a3, A4 a4, A5 a5)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
  
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
    return binder_type::make(mem_f, inst, a, a2, a3, a4, a5); 
}

template<typename F, typename C, typename A, typename A2, 
         typename A3, typename A4, typename A5, typename A6> inline
std::auto_ptr<WorkingQueueEntry> MakeWorkingQueueEntry(F mem_f, C inst, A a, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES(F, C);
  
  BOOST_STATIC_ASSERT(!(boost::is_same<binder_type, Private::ErrorInvalidWorkingQueueEntry>::value));
    return binder_type::make(mem_f, inst, a, a2, a3, a4, a5, a6); 
}

#undef CREATE_WORKINGQUEUEENTRY_OBJECT_FROM_PASSING_TYPES

} // end of asynccallbacks
