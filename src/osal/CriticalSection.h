// this would allow for the use of auto mutex, we have 
// the ctor takes the mutex and the dtor releasing it
// this assumes that we have a function called take
// and a function called give
namespace osal
{
  // all code that belong to OSAL is placed under this namepsace

namespace Private
{
  
template<typename Lock>
struct CriticalSection
{
	CriticalSection(const Lock& l) : mLock(l)
	{
		mLock.Take();
	}
	
	~CriticalSection()
	{
		mLock.Give();
	}
	
private:
	const Lock& mLock;
};

} // end of namespace private

} // end of namespace osal
