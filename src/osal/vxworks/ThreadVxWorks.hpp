#ifndef THREADVXWORKS__HPP
#define THREADVXWORKS__HPP

#include "OsalTimeUtils.h"
#include "CriticalSection.h"		// to guard the error handling functions
#include "ExitFunctionHolder.h"		// to define the list of error handling functions
#include <taskLib.h>	// all the task control functions
#include <vxWorks.h>	// vxworks types
#include <assert.h>   // assert macro
#include <errnoLib.h> // errno for vxworks
#include <string.h>   // strcmp
//#include <stdio.h>    // remove this!!

namespace osal
{
  
namespace Thread
{

namespace
{

const PriorityType DEFAULT_PRIORITY = INVALID_PRIORITY;

bool HandleDeleteErrors(int errno, int line);
Private::ExitFunctionHolder& ExitFunctionsThreadList();
int GetOSPriority(PriorityType prio);
PriorityType FromOSPriority(int priority);


Private::ExitFunctionHolder& ExitFunctionsThreadList()
{
	static Private::ExitFunctionHolder exitFunctions;
	return exitFunctions;
}

bool HandleDeleteErrors(int err, int line)
{
  // if the object is deleted or the id is no longer avaliable ignore the errors
  if (err != S_objLib_OBJ_DELETED && err != S_objLib_OBJ_ID_ERROR)
  {
    ExitFunctionsThreadList().CriticalError(err, "taskDelete", line);
  }
  return false;
}


int GetOSPriority(PriorityType prio)
{
	if (prio != INVALID_PRIORITY)
	{
		return static_cast<int>(prio);	// we are using the same order for priorities here as vxworks's
	}
	else
	{
      // if the priority is not set, than return the one that is used in this task
		int prio = -1;
		taskPriorityGet(taskIdSelf(), &prio);
		return prio;
	}
}

PriorityType FromOSPriority(int priority)
{
  static const PriorityType priorities[] = 
  {
      PRIORITY_1,
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
      PRIORITY_200
  };
  
	if (priority < PRIORITY_1 || priority >= LOWSET_PRIORITY)
	{
		return INVALID_PRIORITY;
	}
	else
	{
		return priorities[priority];
	}
}

}	// end of local namespace

///////////////////////////////////////////////////////////////////////////////

struct Id
{
	static const int INVALID_ID = -1;
	int TaskId;
	
	Id() : TaskId(INVALID_ID)
	{
	}
	
	Id(int id) : TaskId(id)
	{
	}
};

bool operator == (const Id& left, const Id& right)
{
	return left.TaskId == right.TaskId;
}

///////////////////////////////////////////////////////////////////////////////

Attributes::Attributes() : Name(DefaultName()), StackSize(DefaultStackSize()), Priority(DEFAULT_PRIORITY)
{
}

Attributes::Attributes(const char* name, unsigned int stackSize, PriorityType prio) : 
                            Name(name), StackSize(stackSize), Priority(prio)
{
}

Attributes CreateAttribute(const char* name, unsigned int sSize, PriorityType pt)
{
  return Attributes(name, sSize, pt); 
}

void RegisterAtExit(at_error_fun func)
{
	ExitFunctionsThreadList().Push(func);
}

bool EqualsId(Id* left, Id* right)
{
	return ((left == 0) && (right == 0)) || (*left == *right);
}

Id* Create(const Attributes& attr, entry_func_t entryFunc)
{
  
	int tid = taskSpawn((char*)attr.Name, GetOSPriority(attr.Priority), 
	                    VX_FP_TASK, attr.StackSize, (FUNCPTR)entryFunc, 
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            );
	if (tid != ERROR)
	{
	  //printf("created task id %d\r\n", tid);
		return new Id(tid);	// return the new task object
	}
	else
	{
	  ExitFunctionsThreadList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
		return 0; 	// this is just so we would not have warning, it should not return with this NULL!!
	}
}

PriorityType Priority(Id* to)
{
	assert(to);
	int oldPrio = -1;
	if (taskPriorityGet(to->TaskId, &oldPrio) != OK)
	{
	  ExitFunctionsThreadList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
	}
	return FromOSPriority(oldPrio);
}

void NewPriority(Id* to, PriorityType newPrio)
{
  assert(to);
	// do set a new priority if the one pass to this function is is invalid!
	if (newPrio != INVALID_PRIORITY)
	{
	  if (taskPrioritySet(to->TaskId, GetOSPriority(newPrio)) != OK)
		{
		  ExitFunctionsThreadList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
		}
	}
}

const char* Name(Id* id)
{
	assert(id);
	return taskName(id->TaskId);
}

bool Clean(Id*& id)
{
	bool ret = true;
	if (id && taskIdVerify(id->TaskId) == OK)
	{
	  Id me(taskIdSelf());
		if (me == *id)  // this mean just release the resources that we allocated here (not the task itself)
		{
			delete id;   // we can deallocate our task resources, but we cannot delete the task itself
			id = 0;
		}
		else
		{
		  //printf("deleting task id %d\r\n", id->TaskId);
			if (taskDelete(id->TaskId) != OK)
			{
				ret = HandleDeleteErrors(errnoGet(), __LINE__);
			}
			
			if (ret)
			{
				delete id;
				id = 0;
			}
		}
	}
	return ret;
}

bool TimeClean(Id*& id, milliseconds_t milliDuration)
{
	// this would delete the task in any case
	bool ret = true;
	if (!Clean(id))
	{
		Self::Sleep(milliDuration);
		taskDeleteForce(id->TaskId);
		id->TaskId = Id::INVALID_ID;
		ret = false;
		delete id;
	}
	return ret;
}


namespace Self
{

Thread::Id* Id()
{
	return new Thread::Id(taskIdSelf());
}

milliseconds_t Sleep(milliseconds_t milliDuration)
{
  int ticks = TimeUtils::Milli2Ticks(milliDuration);
	taskDelay(ticks);
	return TimeUtils::Ticks2Milli(ticks);
}

void Suspend()
{
	taskDelay(0);
}

const char* Name()
{
  return taskName(taskIdSelf());
}

Thread::PriorityType Priority()
{
	Thread::Id id(taskIdSelf());
	return Priority(&id);
}

void NewPriority(Thread::PriorityType newPrio)
{
  Thread::Id id(taskIdSelf());
  Thread::NewPriority(&id, newPrio);
}

bool Equel(const Thread::Id* other)
{
  return other != 0 && other->TaskId == taskIdSelf();
}

bool EqualName(const char* expectName)
{
  return strcmp(expectName, Thread::Self::Name()) == 0;
}


}	// end of namespace Self

} // end of namespace thread

} // end of namespace osal

#else
#	error "you cannot include this file inside header file"
#endif	// THREADVXWORKS__HPP
