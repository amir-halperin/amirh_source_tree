#pragma once
/**
 * 
 */

namespace osal { namespace Mutex {
  struct Id;
}}  // namespace osal

namespace asynccallbacks
{
  
  
namespace utils
{

  /**
   * @struct CriticalSection
   * use this to protect a given scope with
   * a mutex hanlder passed to the ctor
   * that would lock it, and when exiting the scope
   * the mutex would release automatically no matter
   * how you are exiting this scope
   */
struct CriticalSection
{
  CriticalSection(osal::Mutex::Id* m);
  
  ~CriticalSection();
  
  osal::Mutex::Id* mGuard;
};
  


}   // end of namespace utils
  
}     // end of namespace asynccallbacks
