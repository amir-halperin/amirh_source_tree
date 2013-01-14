#include "osal/OsalGeneralDefines.h"  // general definitions for this package (osal)
#ifdef __VXWORKS__
# include "vxworks/SemHandle.h"  // vxworks lock
#endif  // __VXWROKS__
// all namespace are under namespace osal
namespace osal
{

namespace Private
{

// This would allow to save functions that would be called
// in case of an error - we can register functions that would
// report the errors, function that would return the control
// to some place where we would like to take action over
// the path of exection or anything else
struct ExitFunctionHolder
{
	ExitFunctionHolder();
	
	void Push(at_error_fun fn);
	
	void CriticalError(int err, const char* func, int line) const;
	
	// checks if the error is a result of timeout, if not call CriticalError
	bool TimedOut(int err, const char* func, int line) const;
	
	// checks if the error is that resource not avilable if not call CriticalError
	bool TryFail(int err, const char* func, int line) const;
	
	// this function should call if we just need to exit on so critical condition
	// such as assertion on some dondition that don't allow to continue 
	void Assert(const char* msg, const char* func, int line) const;
	
	// this function is for testing only - we would like to
	// be able to register a function that would not kill
	// the running environment and we would like to be able
	// to ensure that in the tests we can control whether this function
	// was called or not
	void SetDefaultFunction(at_error_fun fn);
	
	// this function would return the default exit function back - 
	// again this is useful for testing where it is needed to set 
	// the system back to the way it was before the test
	static void Reset2Default();
	
protected:
	
private:
	static void LogError(int err, const char* func, int line);
	
	at_error_fun Pop() const;
	
	at_error_fun mList[MAX_FAILURE_CALLBACK_FUNCTIONS];
	mutable unsigned int mCount;
	static at_error_fun DefaultFunction; // this function would be called anytime we have an error on top of functions
                                       // registered by the user
#ifdef __VXWORKS__
	semvx::SemHandle 
#else // for posix put lock here as well
	int
#endif  // __VXWORKS__
                  mLock;
                    
};

#define OSAL_ASSERT_CONDITION(expr, with, msg) ((!(expr) ?  (with).Assert( (msg), __FUNCTION__ , __LINE__) : (void)0));

} // end of namespace Private

} // end of namespace osal
