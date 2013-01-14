/**
 * @file mq_data.cpp
 * 
 * implement the data structures in file mq_data.hpp
 */

#include <boost/message_queue/mq_data.hpp>  // header file for this file

namespace boost
{
  
namespace details
{
  int mq_data::DefaultCtor = 0;
     int mq_data::CtorByProxy = 0;
     int mq_data::CtorWithSize = 0;
     int mq_data::Dtor = 0;
     int mq_data::CopyAssign = 0;
     int mq_data::CopyCtor = 0;
     int mq_data::DtorCalls = 0;
     int mq_data::MemoryAlloc = 0;
      
      
    mq_data_proxy::mq_data_proxy(std::size_t maxSize, const char* msg, std::size_t size) :
                      m_maxSize(maxSize), m_data(msg), m_size(size)
    {
    }
    
    std::size_t mq_data_proxy::capacity() const
    {
      return m_maxSize;
    }
    
    std::size_t mq_data_proxy::size() const
    {
      return m_size;
    }
    
    const char* mq_data_proxy::data() const
    {
      return m_data;
    }
    
    
    mq_data::mq_data()
    {
      ++DefaultCtor;
    }
   
    
    // note that here we are allowing implicit cast and we are not allocting memory at all
    mq_data::mq_data(mq_data_proxy byProxy) :  m_buffer(byProxy.data(), byProxy.size())
    {
      ++CtorByProxy;  
    }
    
    mq_data::mq_data(size_type initial_size)
    {
      MemoryAlloc++;
      ++CtorWithSize;
    }
    
    mq_data::~mq_data()
    {
      ++DtorCalls;
      // make sure that we owned the allocation
      release();
    }
    
    mq_data::mq_data(const mq_data& other) : m_buffer(other.m_buffer)//m_buffer(new char[other.size()])
    {
      MemoryAlloc++;
      ++CopyCtor;
    }
   
    
    mq_data& mq_data::operator = (const mq_data& other)
    {
     if (&other != this)
     {
       ++CopyAssign;
       // we want to make sure that 
       
         // note here that if we had the buffer not allocated localy (assigned from proxy
         // then any data in it would be lost, we are assuming that once 
         // we are re-assign from another we must re-allocate if either 
         // buffer was note allocated before (default constructed)
         // or that we merly pointed to another memory location that we never own
         // but since we must take new data we need to own the internal memory
         // do not re-allocated if we have the memory ready - 
      m_buffer = other.m_buffer;
     }
      return *this;
    }
    
    mq_data::data_type mq_data::data() const
    {
      return data_type(size(), m_buffer.c_str());
    }
    
    mq_data::size_type mq_data::size() const
    {
    return m_buffer.size();
    }
    
    bool mq_data::empty() const
    {
    return m_buffer.empty();
    }
    
    mq_data::iterator mq_data::begin()
    {
        return m_buffer.begin();
    }
    
    mq_data::iterator mq_data::end()
    {
    return m_buffer.end();
    }
    
    mq_data::const_iterator mq_data::begin() const
    {      
      return m_buffer.begin();
    }
    
    mq_data::const_iterator mq_data::end() const
    {
    return m_buffer.end();
    }
    
    mq_data::reference mq_data::operator [] (size_type at)
    {
      return m_buffer[at];
    }
    
    mq_data::const_reference mq_data::operator [] (size_type at) const
    {
      return m_buffer[at];
    }
    
    void mq_data::release()
    {
    }   
   
} // end of namespace Private
} // end of namespace boost

