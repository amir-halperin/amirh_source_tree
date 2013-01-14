#pragma once
/**
@file OsalGeneralDefines.h

This would hold several types and definitions that are shared between all modules
in the OSAL env
*/
namespace osal
{

#ifdef __VXWORKS__  // note that in vxworks the parameter is pass as none const value
#   define CONST_MESSAGE
#else
# ifndef CONST_MESSAGE
#   define CONST_MESSAGE const
# endif // CONST_MESSAGE
#endif  // __VXWORKS__
typedef unsigned long	milliseconds_t;	/* this would be used as timeout value */

typedef void (*at_error_fun)(int);		/* function to be called when something critical happened. 
                                         The user would register a callback function here */
const unsigned int MAX_FAILURE_CALLBACK_FUNCTIONS = 10u;	/* this is the max number of callback functions that can be registered
																by the users of the OSAL lib (per module) */
} // end of namespace osal
