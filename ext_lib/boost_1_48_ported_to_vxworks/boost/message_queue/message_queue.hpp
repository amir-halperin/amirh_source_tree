#pragma once
/**
 * @file message_queue.hpp
 * 
 * This would have a class taht supports a bound buffer that is multi thread safe
 * This is a good replacement for the OS (normally posix by others) message queue
 * the main advantage being that this is a type aware message queue unlink the 
 * othre message queue. Another advantage is that this is not created by the OS so
 * its life time or more pronaounced
 */
#include <boost/message_queue/mq_data.hpp> // message queue data for special cases
#include <boost/circular_buffer.hpp>  // the internal buffer is circular buffer
#include <boost/thread/mutex.hpp>     // class mutex
#include <boost/thread/condition.hpp> // class condition
#include <boost/thread/thread.hpp>    // multi threads
#include <boost/call_traits.hpp>      // optimized function call arguments
#include <boost/progress.hpp>         // timers
#include <boost/bind.hpp>             // bind parameters to function call
#include <boost/date_time/posix_time/posix_time.hpp>  // for duration
#include <algorithm>                  // min

/**
 * @class MTBoundBuffer
 * @brief a message queue that support multi readers/writes to a queue with given size
 * 
 * This class can serves as replacement for the OS message queue
 * IT has 2 adavantages over it - 
 * 1. it is type aware (where in OS message queue you are placing void* and you are bound by size)
 * 2. it has a better life time - with OS message queue you have to manage it yourself
 *  note that this code was take from boost example 
 *  http://www.boost.org/doc/libs/1_48_0/libs/circular_buffer/doc/circular_buffer.html#interprocess
 */
namespace boost
{
  

  
class message_queue_base
{
public:
  
  std::size_t size() const
  {
    return m_unread;
  }
  
   bool is_not_empty() const 
   { 
     return m_unread > 0; 
   }
  
protected:
   

   explicit message_queue_base();
   
   ///////////////////////////////////////////////
   // helper calls for the guarding the data
   struct push_op
   {
     
     template<typename F>
     push_op(boost::timed_mutex& m, boost::condition& nf, boost::condition& ne, F condition) : 
                                           m_mutex(m), m_not_full(nf), m_not_empty(ne)
     {
       m_not_full.wait(m_mutex, condition);
     }
     
     ~push_op();
     
   private:
     boost::unique_lock<boost::timed_mutex> m_mutex;
     boost::condition& m_not_full;
     boost::condition& m_not_empty;
   };
   
   struct time_push
   {
     time_push(boost::timed_mutex& m, boost::condition& nf, boost::condition& ne);                       
     
     template<typename TO, typename F>
     bool enter(const TO& timeout, F condition)
     {
       if (m_mutex.timed_lock(timeout))
       {
         m_release = true;
         if (m_not_full.timed_wait(m_mutex, timeout, condition))
         {
           return true;
         }
       } 
       return false;
     }
     
     ~time_push();
     
     
     void leave(bool notify);
     
   private:
     boost::unique_lock<boost::timed_mutex> m_mutex;
     boost::condition& m_not_full;
     boost::condition& m_not_empty;
     bool m_release;
   };
   
   struct pop_op
   {
     
     template<typename F>
     pop_op(boost::timed_mutex& m, boost::condition& nf, boost::condition& ne, F condition) : 
                                           m_mutex(m), m_not_full(nf), m_not_empty(ne)
     {
      m_not_empty.wait(m_mutex, condition);
     }
     
     ~pop_op();
     
   private:
     boost::unique_lock<boost::timed_mutex> m_mutex;
     boost::condition& m_not_full;
     boost::condition& m_not_empty;
   };
   
   struct time_pop
   {
     time_pop(boost::timed_mutex& m, boost::condition& nf, boost::condition& ne); 
                                           
    
     template<typename TO, typename F>
     bool enter(const TO& timeout, F condition)
     {
       
       if (m_mutex.timed_lock(timeout))
       {
       
         m_release = true;
         if (m_not_empty.timed_wait(m_mutex, timeout, condition))
         { 
         
           return true;
         }
       }
       return false;
     }
     
     void leave(bool notify);
     
     ~time_pop();
       
   private:
     boost::unique_lock<boost::timed_mutex> m_mutex;
     boost::condition& m_not_full;
     boost::condition& m_not_empty;
     bool m_release;
   };
   
     ///////////////////////////////////////
   
protected:
   std::size_t m_unread;
   boost::timed_mutex m_mutex;
   boost::condition m_not_empty;
   boost::condition m_not_full;
};
  
template <class T>
class message_queue : protected message_queue_base
{
  typedef message_queue_base base_type;
public:
  typedef boost::circular_buffer<T> container_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::value_type value_type;
  typedef typename boost::call_traits<value_type>::param_type param_type;


   /**
    * constructor for the class accept the size
    * @param size the max elements that can be inserted into the buffer
    */
   explicit message_queue(size_type capacity) : m_container(capacity)
   {
   }

   /**
    * @function push_front
    * @brief add new element to the buffer
    * @param item the new item to add. Note that if the queue is full this would blocked the calling thread
    */
   void push_front(const value_type& item) 
   {
      // param_type represents the "best" way to pass a parameter of type value_type to a method
      //boost::mutex::scoped_lock lock(m_mutex, boost::try_to_lock_t());
      base_type::push_op guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty, 
                               boost::bind(&message_queue<value_type>::is_not_full, this)); 
      m_container.push_front(item);
      ++base_type::m_unread;
   }
   
   /**
    * @function try_push
    * @brief try to add new element to the queue - if not locked (otherwise same as push_front)
    * @param item which is the new item to add to the queue
    * @return true if item added else return false
    */
   bool try_push(const value_type& item)
   {
     // only if we can access the lock here we would try and push new item inside
     if (this->is_not_full())  // this call is thread safe..
     {
       this->push_front(item);
       return true;
     }
     else
     {
       return false;
     }
   }
   
   /**
    * @function try_push
    * @brief try to add new item with a timeout value (that is unlike the other try function this relay on timeout)
    * @param item the new item to push in to the queue
    * @param timeout the timeout value (duration) that we would wait for, if passed we would return false
    * @return true if item added, else return false
    */
   bool try_push(const value_type& item, const boost::posix_time::time_duration & timeout)
   {
     base_type::time_push guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&message_queue<value_type>::is_not_full, this)))
     {
         m_container.push_front(item);
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

   /**
    * @function try_push
    * @brief try to add new item with a timeout value (that is unlike the other try function this relay on timeout)
    * @param item the new item to push in to the queue
    * @param timeout the timeout value (absolute) that we would wait for, if passed we would return false
    * @return true if item added, else return false
    */
   bool try_push(const value_type& item, const boost::system_time & timeout)
   {
     base_type::time_push guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&message_queue<value_type>::is_not_full, this)))
     {
         m_container.push_front(item);
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
   /**
    * @function pop_back
    * @brief get the upper most (oldest) element in the buffer
    * @param item, pointer to item to which the data would be stored 
    */
   void pop_back(value_type* item) 
   {
      //boost::mutex::scoped_lock lock(m_mutex, boost::try_to_lock_t());
      base_type::pop_op guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty,
                              boost::bind(&base_type::is_not_empty, this));
      *item = m_container[--base_type::m_unread];
   }

   /**
    * @function try_pop
    * @brief get the upper most (oldest) element in the buffer - if not empty
    * @param item, pointer to item to which the data would be stored 
    * @return true if item was poped
    */
   bool try_pop(value_type* item) 
   {
     if (this->is_not_empty())
     {
       this->pop_back(item);
       return true;
     }
     else
     {
       return false;
     }
   }

   /**
    * @function try_pop
    * @brief get the upper most (oldest) element in the buffer - if not empty
    * @param item, pointer to item to which the data would be stored 
    * @param timeout reletive time out value (time is taken from the time of the call)
    * @return true if item was poped
    */
   bool try_pop(value_type* item, const boost::posix_time::time_duration& timeout) 
   {
     base_type::time_pop guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, timeout, boost::bind(&base_type::is_not_empty, this)))
     {
       *item = m_container[--base_type::m_unread];
       guard.leave(true);
       return true;
     }
     else
     {
       guard.leave(false);
       return false;
     }
   }
   
   /**
    * @function try_pop
    * @brief get the upper most (oldest) element in the buffer - if not empty
    * @param item, pointer to item to which the data would be stored 
    * @param timeout - an abs timeout value (that is it is not relative to the time of the call)
    * @return true if item was poped
    */
   bool try_pop(value_type* item, const boost::system_time& timeout) 
   {
     base_type::time_pop guard(base_type::m_mutex, base_type::m_not_full, base_type::m_not_empty);
     
     if (guard.enter(timeout, boost::bind(&base_type::is_not_empty, this)))
     {
       *item = m_container[--base_type::m_unread];
       guard.leave(true);
       return true;
     }
     else
     {
       guard.leave(false);
       return false;
     }
   }
   
private:
   message_queue(const message_queue&);              // Disabled copy constructor
   message_queue& operator = (const message_queue&); // Disabled assign operator

   
   bool is_not_full() const 
   { 
     return m_unread < m_container.capacity(); 
   }

   container_type m_container;
};

/**
 * @class message_queue
 * @brief optimazied version that accept only opaque data by mean of char* and size
 * this is an optimized version that would make sure that we are not just using 
 * allocated memory on the fly - it would allow for better interation with C lagacy code
 * we would only be using char* as opaque data and the size to save to the message queue 
 */
template<>
class message_queue<details::mq_data> : public message_queue_base
{
  typedef message_queue_base base_type;
public:
    typedef boost::circular_buffer<details::mq_data> container_type;
    typedef  container_type::size_type size_type;
    typedef container_type::value_type value_type;
    typedef  message_queue<details::mq_data>  this_type;

   /**
    * constructor for the class accept the size
    * @param size the max elements that can be inserted into the buffer
    */
   explicit message_queue(size_type capacity, size_type maxMessageSize);
   
   ~message_queue();
   /*{
     while (!m_container.empty())
     {
       m_container.pop_front(); // just remove all the variable from here so that they would be destructed
     }
   }*/
   
   /**
    * @function push_front
    * @brief add new element to the buffer
    * @param item the new item to add. Note that if the queue is full this would blocked the calling thread
    */
   void push_front(const char* item, std::size_t size);
  
   
   /**
    * @function try_push
    * @brief try to add new element to the queue - if not locked (otherwise same as push_front)
    * @param item which is the new item to add to the queue
    * @return true if item added else return false
    */
   bool try_push(const char* item, std::size_t size);
   
   /**
    * @function try_push
    * @brief try to add new item with a timeout value (that is unlike the other try function this relay on timeout)
    * @param item the new item to push in to the queue
    * @param timeout the timeout value (duration) that we would wait for, if passed we would return false
    * @return true if item added, else return false
    */
   bool try_push(const char* item, std::size_t size, const boost::posix_time::time_duration & timeout);

   /**
    * @function try_push
    * @brief try to add new item with a timeout value (that is unlike the other try function this relay on timeout)
    * @param item the new item to push in to the queue
    * @param timeout the timeout value (absolute) that we would wait for, if passed we would return false
    * @return true if item added, else return false
    */
   bool try_push(const char* item, std::size_t size, const boost::system_time & timeout);
   /**
    * @function pop_back
    * @brief get the upper most (oldest) element in the buffer
    * @param item, pointer to item to which the data would be stored 
    */
   void pop_back(char* item, std::size_t& size); 

   /**
    * @function try_pop
    * @brief get the upper most (oldest) element in the buffer - if not empty
    * @param item, pointer to item to which the data would be stored 
    * @return true if item was poped
    */
   bool try_pop(char* item, std::size_t& size); 
   
   /**
    * @function try_pop
    * @brief get the upper most (oldest) element in the buffer - if not empty
    * @param item, pointer to item to which the data would be stored 
    * @param timeout reletive time out value (time is taken from the time of the call)
    * @return true if item was poped
    */
   bool try_pop(char* item, std::size_t& size, const boost::posix_time::time_duration& timeout); 
   
   /**
    * @function try_pop
    * @brief get the upper most (oldest) element in the buffer - if not empty
    * @param item, pointer to item to which the data would be stored 
    * @param timeout - an abs timeout value (that is it is not relative to the time of the call)
    * @return true if item was poped
    */
   bool try_pop(char* item, std::size_t& size, const boost::system_time& timeout);
   
private:
   message_queue(const message_queue&);              // Disabled copy constructor
   message_queue& operator = (const message_queue&); // Disabled assign operator

   
   bool is_not_full() const 
   { 
     return m_unread < m_container.capacity(); 
   }
   
   container_type m_container;
   size_type      m_messageSize;
};

typedef message_queue<details::mq_data> opaque_message_queue; // just to make the use of the above queue easier

} // end of namespace boost
