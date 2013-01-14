#pragma once
/**
@file thread.h
This is the user interface for thread/tasks creation.
The name " thread" selected over task since this is more
common (in fact the only operation system the chooses tasks as far
as I know is VxWorks). It is also more common name in other programming
languages and in many documentation of this issue. Other than the name 
effect is the same. Tasks/threads are independent from each other in 
their scheduling and stack variables. But they do share the same stack
from which they started and they do share the same global and static 
(function static) variables.
There is one big difference between the tasks in VxWorks
and the threads in POSIX (and other OSs) - in VxWorks
there is no way you can wait on a task to complete, this is 
however important (and sometimes a must) in POSIX and
other OSs. Since this option do not exists in VxWorks
it would be optional option to have in this interface. Since
we are not using this option today we can live with this
assumption. Note also that in VxWorks the type of 
the function that is pass to the OS as the starting
point to the task is different from that in POSIX.
In POSIX this function type 
void* (*)(void*)
while in VxWorks it is 
void (*)()
That is - a function that accepts no parameters and 
return nothing, while in POSIX the function is a function
that return void pointer and accept void pointer
In VxWorks the taskSpawn is called with 10 arguments that the
entry function can use (they are not part of the entry
function arguments but rather a task lock storage)
Since this is not the same at all - we would assume for POSIX
that all functions are not returning a value and accept nothing
that is the entry function is just like in VxWorks
In C++ interface the argument restriction would be lifted (in C++
interface you can pass any function and the return value is 
ignored). We should however have some way to identify a task
again this is a bit different in POSIX and VxWorks - 
POSIX don't have standard for thread names, so ID is 
the only way to identify threads
*/

#include "osal/OsalGeneralDefines.h"


namespace osal
{

namespace Thread
{

  
  inline const char* DefaultName()
  {
    static const char* THREAD_WITHOUT_NAME = "TaskNoName"; // note that if no name is given this is the name that would be used
    return THREAD_WITHOUT_NAME;
  }
 
  inline unsigned int DefaultStackSize()
  {
    static const unsigned int DEFAULT_THREAD_STACK_SIZE = 16 * 1024;
    return DEFAULT_THREAD_STACK_SIZE;
  }
/** please note that we would list priority by "numbers"
 that is - priorities are numbered here in the enumerations
 don't use explicit numbers to set the priorities
*/
enum PriorityType
{
	HIGHEST_PRIORITY = 0,
	PRIORITY_1 = HIGHEST_PRIORITY,
	PRIORITY_2,
	PRIORITY_3,
	PRIORITY_4,
	PRIORITY_5,
	PRIORITY_6,
	PRIORITY_7,
	PRIORITY_8,
	PRIORITY_9,
	PRIORITY_10,
	PRIORITY_11,
	PRIORITY_12 ,
	PRIORITY_13 ,
	PRIORITY_14 ,
	PRIORITY_15 ,
	PRIORITY_16 ,
	PRIORITY_17 ,
	PRIORITY_18 ,
	PRIORITY_19 ,
	PRIORITY_20 ,
	PRIORITY_21 ,
	PRIORITY_22 ,
	PRIORITY_23 ,
	PRIORITY_24 ,
	PRIORITY_25 ,
	PRIORITY_26 ,
	PRIORITY_27 ,
	PRIORITY_28 ,
	PRIORITY_29 ,
	PRIORITY_30 ,
	PRIORITY_31 ,
	PRIORITY_32 ,
	PRIORITY_33 ,
	PRIORITY_34 ,
	PRIORITY_35 ,
	PRIORITY_36 ,
	PRIORITY_37 ,
	PRIORITY_38 ,
	PRIORITY_39,
	PRIORITY_40,
	PRIORITY_41,
	PRIORITY_42,
	PRIORITY_43,
	PRIORITY_44,
	PRIORITY_45,
	PRIORITY_46,
	PRIORITY_47,
	PRIORITY_48,
	PRIORITY_49,
	PRIORITY_50,
	PRIORITY_51,
	PRIORITY_52,
	PRIORITY_53,
	PRIORITY_54,
	PRIORITY_55,
	PRIORITY_56,
	PRIORITY_57,
	PRIORITY_58,
	PRIORITY_59,
	PRIORITY_60,
	PRIORITY_61,
	PRIORITY_62,
	PRIORITY_63,
	PRIORITY_64,
	PRIORITY_65,
	PRIORITY_66,
	PRIORITY_67,
	PRIORITY_68,
	PRIORITY_69,
	PRIORITY_70,
  PRIORITY_71,
  PRIORITY_72,
  PRIORITY_73,
  PRIORITY_74,
  PRIORITY_75,
  PRIORITY_76,
  PRIORITY_77,
  PRIORITY_78,
  PRIORITY_79,
  PRIORITY_80,
  PRIORITY_81,
  PRIORITY_82,
  PRIORITY_83,
  PRIORITY_84,
  PRIORITY_85,
  PRIORITY_86,
  PRIORITY_87,
  PRIORITY_88,
  PRIORITY_89,
  PRIORITY_90,
  PRIORITY_91,
  PRIORITY_92,
  PRIORITY_93,
  PRIORITY_94,
  PRIORITY_95,
  PRIORITY_96,
  PRIORITY_97,
  PRIORITY_98,
  PRIORITY_99,
  PRIORITY_100,
  PRIORITY_101,
  PRIORITY_102,
  PRIORITY_103,
  PRIORITY_104,
  PRIORITY_105,
  PRIORITY_106,
  PRIORITY_107,
  PRIORITY_108,
  PRIORITY_109,
  PRIORITY_110,
  PRIORITY_111,
  PRIORITY_112,
  PRIORITY_113,
  PRIORITY_114,
  PRIORITY_115,
  PRIORITY_116,
  PRIORITY_117,
  PRIORITY_118,
  PRIORITY_119,
  PRIORITY_120,
  PRIORITY_121,
  PRIORITY_122,
  PRIORITY_123,
  PRIORITY_124,
  PRIORITY_125,
  PRIORITY_126,
  PRIORITY_127,
  PRIORITY_128,
  PRIORITY_129,
  PRIORITY_130,
  PRIORITY_131,
  PRIORITY_132,
  PRIORITY_133,
  PRIORITY_134,
  PRIORITY_135,
  PRIORITY_136,
  PRIORITY_137,
  PRIORITY_138,
  PRIORITY_139,
  PRIORITY_140,
  PRIORITY_141,
  PRIORITY_142,
  PRIORITY_143,
  PRIORITY_144,
  PRIORITY_145,
  PRIORITY_146,
  PRIORITY_147,
  PRIORITY_148,
  PRIORITY_149,
  PRIORITY_150,
  PRIORITY_151,
  PRIORITY_152,
  PRIORITY_153,
  PRIORITY_154,
  PRIORITY_155,
  PRIORITY_156,
  PRIORITY_157,
  PRIORITY_158,
  PRIORITY_159,
  PRIORITY_160,
  PRIORITY_161,
  PRIORITY_162,
  PRIORITY_163,
  PRIORITY_164,
  PRIORITY_165,
  PRIORITY_166,
  PRIORITY_167,
  PRIORITY_168,
  PRIORITY_169,
  PRIORITY_170,
  PRIORITY_171,
  PRIORITY_172,
  PRIORITY_173,
  PRIORITY_174,
  PRIORITY_175,
  PRIORITY_176,
  PRIORITY_177,
  PRIORITY_178,
  PRIORITY_179,
  PRIORITY_180,
  PRIORITY_181,
  PRIORITY_182,
  PRIORITY_183,
  PRIORITY_184,
  PRIORITY_185,
  PRIORITY_186,
  PRIORITY_187,
  PRIORITY_188,
  PRIORITY_189,
  PRIORITY_190,
  PRIORITY_191,
  PRIORITY_192,
  PRIORITY_193,
  PRIORITY_194,
  PRIORITY_195,
  PRIORITY_196,
  PRIORITY_197,
  PRIORITY_198,
  PRIORITY_199,
  PRIORITY_200,
	LOWSET_PRIORITY,     // dont move this one
	INVALID_PRIORITY = -1		// dont add anything bellow this one
};

struct Id;

/**
@struct ThreadAttributes
@brief arguments passed when creating a thread

This would be used to pass arguments for the
thread when starting it. These include the thread name
the thread stack size and the thread priority
it is advisable to not pass the last two and 
to leave them as defaults (as it would make
the code more portable
*/
struct Attributes
{
	const char* Name;
	unsigned int StackSize;
	PriorityType Priority;

	/// default ctor
	Attributes();
	
	/**
		@brief constract this object with parameters
		@param name the thread name
		@param stackSize the size of the stakc in bytes
		@param prio the priotity of the thread - if this is set to INVALID_PRIORITY, then use the creating task priority
		@param detach only on Linux allow this thread to run without the to wait for its compliation
	*/
	Attributes(const char* name, unsigned int stackSize,
				PriorityType prio);
};

Attributes CreateAttribute(const char* name, unsigned int sSize, PriorityType);

/**
@brief this function would register a callback function to be called when critical error has happened
@param func the function to be called at exit
*/
void RegisterAtExit(at_error_fun func);

/** 
define the type of the entry function to be used
when starting the thread - this is the function that
would be called by the OS when the thread is starting 
*/
typedef void (*entry_func_t)();

/**
@brief compare two thread IDs for equality
@param left the left hand ID
@param right the right hand ID
@return true value if they have the same ID
*/
bool EqualsId(Id* left, Id* right);

/**
@brief create and start a new thread
This function is the same as taskSpawn in Vxworks and pthread_create - 
that is the call to this function would start a new thread of execution with
the given arguments and the entry point from which the code in the new 
thread would started is the function that pass as argument to this function
note that this function cannot fail, and there is no need to check for NULLness
@param attr are the thread attributes - see the documents for ThreadAttributes
@param entryFunc is the function that would be run when the thread starting the thread
@return pointer to ThreadId
*/
Id* Create(const Attributes& attr, entry_func_t entryFunc);

/**
@brief This function would return the priority of the given thread
@param to the thread that we would like to set the priority to
@return the old priority
*/
PriorityType Priority(Id* to);

/**
 * @brief set a new priority to the thread with a given id
 * @param to the thread to which the priority is set
 * @param newPrio the new priority to set
 */
void NewPriority(Id* to, PriorityType newPrio);

/**
@brief this function would return the thread name
@param id the id of the thread that we would like to get its name
@return the thread name, if ID is valid, else NULL
*/
const char* Name(Id* id);

/**
@brief call this function when the thread is no longer needed
This function is different for VXWorks and POSIX under the hood -
Under POSIX if the thread is create as detachable the calling 
to this function would block the current thread until the thread 
with the given ID would finish, then this would deallocate all
resources allocated for the thread that we are deleting
For VxWorks this would call the taskDelete function. Since the taskDelete
may not actually stop the other thread the return value must be checked
@param id the id of the thread that need to be de-allocated
@return true value if successfully de-allocated resources of this thread
*/
bool Clean(Id*& id);

/**
@brief to this function would block the current thread until the thread 
with the given ID would finish or the timeout has passed, then this would de-allocate all
resources allocated for the thread that we are deleting
For VxWorks this would call the taskDelete function. Since the taskDelete
may not actually stop the other thread the return value must be checked. Note that
if the time was pass we would force the deletion of the task! so this would
always succeed under VxWorks
@param id the id of the thread that need to be de-allocated
@param milliDuration time to wait for the thread to exit by itself
@return true value if successfully de-allocated resources of this thread
*/
bool TimeClean(Id*& id, milliseconds_t milliDuration);

/**
 * @brief collection of functions that would be used to operate on the current thread
 * This is a collection of functions that do not need to have the thread id as parameter
 * as they are working on the current thread. thay are put in their own namespace so
 * that it would be clear that they are more special case for this sole purpose
 */
namespace Self
{
/**
@return the ID of the current thread 
*/
Thread::Id* Id();

/**
@brief put the calling thread to sleep
@param sleep time in milli seconds
@return the number of milliseconds that it was actually going to sleep (due to rounding)
*/
milliseconds_t Sleep(milliseconds_t milliDuration);

/**
@brief suspend the calling thread (that is yield the CPU to other threads)
*/
void Suspend();

/**
@brief the priority of the thread from which the function was called 
@return the old priority
*/
Thread::PriorityType Priority();

/**
 * @brief this function would set a new priority to the current thread
 * This function would set a new priority to the current thread
 * @param newPrio the new priority to the current thread
 */
void NewPriority(Thread::PriorityType newPrio);

/**
@brief this function would return the current thread name
@return the thread name
*/
const char* Name();

/**
 * @brief this would return true if the current thread and other thread are the same
 * @param other the other thread id to compare
 */
bool Equel(const Thread::Id* other);

/**
 * @brief return true if this thread name having the same name as the name given
 * @param expectName the expected name for this thread 
 */
bool EqualName(const char* expectName);

}   // end of Self namespace

}   // end of namespace Thread





} // end of namespace osal
