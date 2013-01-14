#pragma once
/**
 * @file mq_data.hpp
 * this would have a data type that can be used to optimized the message sending
 * from and to a message queue in the sence that we would eliminate data allocation
 * this is based on the observetion that OS message queues are using data as opaque
 * data and are not aware of how this data is arange or what it is
 */
#include <utility>  // STL pair
#include <string>

namespace boost
{

namespace details
{
/**
 * @class mq_data_proxy
 * @brief allow to pass the data of some pre-allocated memory
 * IMPORTANT - this is for internal use only!
 */
class mq_data_proxy
{ 
public:
  // note that this the only option to create and set the data here
  mq_data_proxy(std::size_t maxSize, const char* msg, std::size_t size);
  
  std::size_t capacity() const;
  
  std::size_t size() const;
  
  const char* data() const;
  
private:
  std::size_t m_maxSize;
  const char* m_data;
  std::size_t m_size;
};

/**
 * @class mq_data
 * @brief a buffer to hold data inside it
 * 
 * This would eliminate the need to re-allocate data by mean of perallocation
 * and then just swap a content from and to buffer, to facilitate this
 * we would use move operations
 * IMPORTANT - this is for internal use only!
 */
class mq_data
{
public:
  // standard typedefs
  typedef char*                       value_type;
  typedef std::string::iterator       iterator;
  typedef std::string::const_iterator const_iterator;
  typedef std::string::pointer        pointer;
  typedef std::string::const_pointer  const_pointer;
  typedef std::string::reference      reference;
  typedef std::string::const_reference const_reference;
  typedef std::ptrdiff_t              difference_type;
  typedef std::size_t                 size_type;
  
  typedef std::pair<size_type, const char*> data_type;
 
  mq_data();
 
  
  // note that here we are allowing implicit cast and we are not allocting memory at all
  mq_data(mq_data_proxy byProxy);
  
  explicit mq_data(size_type initial_size);
  
  ~mq_data();
  
  mq_data(const mq_data& other);
  
  mq_data& operator = (const mq_data& other);
  
  data_type data() const; 
  
  size_type size() const;
  
  bool empty() const;
  
  iterator begin();
  
  iterator end();
  
  const_iterator begin() const;
  
  const_iterator end() const;
  
  reference operator [] (size_type at);
  
  const_reference operator [] (size_type at) const;
  
  void release();
  
private:
  std::string m_buffer;
public:
  static int DefaultCtor;
  static int CtorByProxy;
  static int CtorWithSize;
  static int Dtor;
  static int CopyAssign;
  static int CopyCtor;
  static int DtorCalls;
  static int MemoryAlloc;

};

} // end of namespace Private

} // end of namespace boost
