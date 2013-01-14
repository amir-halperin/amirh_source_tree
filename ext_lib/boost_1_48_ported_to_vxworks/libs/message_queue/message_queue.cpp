/**
 * @file message_queue.cpp
 * @brief implement header file message_queue.hpp
 */
#include <boost/message_queue/message_queue.hpp>  // header file for this CPP file

namespace boost
{
  message_queue_base::message_queue_base() : m_unread(0)
  {
    
  }
  
  message_queue_base::push_op::~push_op()
  {
    m_mutex.unlock();
    m_not_empty.notify_one();
  }
  
  message_queue_base::time_push::time_push(boost::timed_mutex& m, boost::condition& nf, boost::condition& ne) :
    m_mutex(m, boost::defer_lock_t()), m_not_full(nf), m_not_empty(ne), m_release(false)
  {
  }
  message_queue_base::time_push::~time_push()
  {
    if (m_release)
    {
      m_mutex.unlock();
    }
  }
  
  void message_queue_base::time_push::leave(bool notify)
  {
    if (m_release)
    {
      m_mutex.unlock();
      m_release = false;
    }
    if (notify)
    {
       m_not_empty.notify_one();
    }
  }
  
  message_queue_base::pop_op::~pop_op()
  {
    m_mutex.unlock();
    m_not_full.notify_one();    
  }
  
  message_queue_base::time_pop::time_pop(boost::timed_mutex& m, boost::condition& nf, boost::condition& ne) :
                                               m_mutex(m, boost::defer_lock_t()), 
                                               m_not_full(nf), m_not_empty(ne), m_release(false)
  {
  }
  
  message_queue_base::time_pop::~time_pop()
  {
    if (m_release)
    {
      m_mutex.unlock();
    }    
  }
  
  void message_queue_base::time_pop::leave(bool notify)
  {
    if (m_release)
    {
      m_mutex.unlock();
      m_release = false;
    }
    if (notify)
    {
      m_not_full.notify_one();
    }    
  }
///////////////////////////////////////////////////////////////////////////////
  
  //template<>
  message_queue<details::mq_data>::message_queue(size_type capacity, size_type maxMessageSize) :  
                    m_container(capacity), m_messageSize(maxMessageSize)
  { 
  }
  
  message_queue<details::mq_data>::~message_queue()
  {
    m_mutex.lock();
    for (container_type::iterator i = m_container.begin(); i != m_container.end(); i++)
    {
      i->release();
    }
    m_mutex.unlock();
    //m_not_full.notify_all();
    //m_not_empty.notify_all();
    
  }
  
   void message_queue<details::mq_data>::push_front(const char* item, std::size_t size) 
   {
      // param_type represents the "best" way to pass a parameter of type value_type to a method
      //boost::mutex::scoped_lock lock(m_mutex, boost::try_to_lock_t());
      base_type::push_op guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty, 
                               boost::bind(&this_type::is_not_full, this)); 
      
      m_container.push_front(details::mq_data_proxy(m_messageSize, item, size));
      ++base_type::m_unread;
   }
   
   
   bool message_queue<details::mq_data>::try_push(const char* item, std::size_t size)
   {
     // only if we can access the lock here we would try and push new item inside
     if (this->is_not_full())  // this call is thread safe..
     {
       this->push_front(item, size);
       return true;
     }
     else
     {
       return false; 
     }
   }
   
   bool message_queue<details::mq_data>::try_push(const char* item, std::size_t size, const boost::posix_time::time_duration & timeout)
   {
     base_type::time_push guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&this_type::is_not_full, this)))
     {
         m_container.push_front(details::mq_data_proxy(m_messageSize, item, size));
         ++m_unread;
         guard.leave(true);
         return true;
     }
     else
     {
       guard.leave(false);
       return false;
     }
   }
   
   
   bool message_queue<details::mq_data>::try_push(const char* item, std::size_t size, const boost::system_time & timeout)
   {
     base_type::time_push guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&this_type::is_not_full, this)))
     {
         m_container.push_front(details::mq_data_proxy(m_messageSize, item, size));
         ++base_type::m_unread;
         guard.leave(true);
         return true;
     }
     else
     {
       guard.leave(false);
       return false;
     }
   }
   
   void message_queue<details::mq_data>::pop_back(char* item, std::size_t& size) 
   {
      //boost::mutex::scoped_lock lock(m_mutex, boost::try_to_lock_t());
      base_type::pop_op guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty,
                              boost::bind(&base_type::is_not_empty, static_cast<const base_type*>(this)));
      details::mq_data::data_type i = m_container[--base_type::m_unread].data();
      size = std::min(i.first, size); // make sure that we didn't overflow
      std::copy(i.second, i.second+size, item);
   }
   
   
   bool message_queue<details::mq_data>::try_pop(char* item, std::size_t& size) 
   {
     if (this->is_not_empty())
     {
       this->pop_back(item, size);
       return true;
     }
     else
     {
       return false;
     }
   }
   
   bool message_queue<details::mq_data>::try_pop(char* item, std::size_t& size, const boost::posix_time::time_duration& timeout) 
   {
     base_type::time_pop guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&base_type::is_not_empty, this)))
     {
       details::mq_data::data_type i = m_container[--base_type::m_unread].data();
       size = std::min(i.first, size); // make sure that we didn't overflow
       std::copy(i.second, i.second+size, item);       
       return true;
     }
     else
     {
       guard.leave(false);
       return false;
     }
   }
   
   
   bool message_queue<details::mq_data>::try_pop(char* item, std::size_t& size, const boost::system_time& timeout)
   {
     base_type::time_pop guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&base_type::is_not_empty, this)))
     {
       details::mq_data::data_type i = m_container[--base_type::m_unread].data();
       size = std::min(i.first, size); // make sure that we didn't overflow
       std::copy(i.second, i.second+size, item);
       guard.leave(true);
       return true;
     }
     else
     {
       guard.leave(false);
       return -1;
     }
   }
} // end of namespace boost
