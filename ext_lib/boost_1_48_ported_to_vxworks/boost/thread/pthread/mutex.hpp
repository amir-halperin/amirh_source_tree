#ifndef BOOST_THREAD_PTHREAD_MUTEX_HPP
#define BOOST_THREAD_PTHREAD_MUTEX_HPP
// (C) Copyright 2007-8 Anthony Williams
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <pthread.h>
#include <boost/utility.hpp>
#include <boost/throw_exception.hpp>
#include <boost/thread/exceptions.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/assert.hpp>
#include <errno.h>
#include "timespec.hpp"
#include "pthread_mutex_scoped_lock.hpp"

#ifdef _POSIX_TIMEOUTS
#if _POSIX_TIMEOUTS >= 0
#define BOOST_PTHREAD_HAS_TIMEDLOCK
#endif
#endif

#include <boost/config/abi_prefix.hpp>

namespace boost
{
  
    class mutex
    {
    private:
        mutex(mutex const&);
        mutex& operator=(mutex const&);        
        pthread_mutex_t m;
    public:
        mutex(
#if defined(__VXWORKS__)
            bool enable_prio_int = true
#endif  // __VXWORKS__
            )
        {
          int res = 0;
#if defined(__VXWORKS__)
            if (enable_prio_int)
            {
              pthread_mutexattr_t att;
              pthread_mutexattr_init(&att);
              pthread_mutexattr_setprotocol(&att, PTHREAD_PRIO_INHERIT);              
              res=pthread_mutex_init(&m,&att);
              pthread_mutexattr_destroy(&att);
            }
            else
            {
              res=pthread_mutex_init(&m, NULL);
            }
#else
            res=pthread_mutex_init(&m, NULL);
#endif  // __VXWORKS__
            if(res)
            {
                boost::throw_exception(thread_resource_error());
            }
        }
        ~mutex()
        {
            BOOST_VERIFY(!pthread_mutex_destroy(&m));
        }
        
        void lock()
        {
            int const res=pthread_mutex_lock(&m);
            if(res)
            {
                boost::throw_exception(lock_error(res));
            }
        }

        void unlock()
        {
            BOOST_VERIFY(!pthread_mutex_unlock(&m));
        }
        
        bool try_lock()
        {
            int const res=pthread_mutex_trylock(&m);
            if(res && (res!=EBUSY))
            {
                boost::throw_exception(lock_error(res));
            }
            
            return !res;
        }

        typedef pthread_mutex_t* native_handle_type;
        native_handle_type native_handle()
        {
            return &m;
        }

        typedef unique_lock<mutex> scoped_lock;
        typedef detail::try_lock_wrapper<mutex> scoped_try_lock;
    };

    typedef mutex try_mutex;

    class timed_mutex
    {
    private:
        timed_mutex(timed_mutex const&);
        timed_mutex& operator=(timed_mutex const&);        
    private:
        pthread_mutex_t m;
#ifndef BOOST_PTHREAD_HAS_TIMEDLOCK
        pthread_cond_t cond;
        bool is_locked;
#endif
    public:
        timed_mutex(
#if defined(__VXWORKS__)
            bool enable_prio_int = true
#endif  // __VXWORKS__
        )
        {
          
          int res = 0;
#if defined(__VXOWRKS__)
         if (enable_prio_int) 
         {
           pthread_mutexattr_t att;
           pthread_mutexattr_init(&att);
           pthread_mutexattr_setprotocol(&att, PTHREAD_PRIO_INHERIT);
           res=pthread_mutex_init(&m,&att);
           pthread_mutexattr_destroy(&att);
         else
         {
           res=pthread_mutex_init(&m,NULL);
         }
#else
           res=pthread_mutex_init(&m,NULL);
#endif  // __VXWORKS__ 
         if(res)
         {
             boost::throw_exception(thread_resource_error());
         }
#ifndef BOOST_PTHREAD_HAS_TIMEDLOCK
            int const res2=pthread_cond_init(&cond,NULL);
            if(res2)
            {
                BOOST_VERIFY(!pthread_mutex_destroy(&m));
                boost::throw_exception(thread_resource_error());
            }
            is_locked=false;
#endif
        }
        ~timed_mutex()
        {
            BOOST_VERIFY(!pthread_mutex_destroy(&m));
#ifndef BOOST_PTHREAD_HAS_TIMEDLOCK
            BOOST_VERIFY(!pthread_cond_destroy(&cond));
#endif
        }

        template<typename TimeDuration>
        bool timed_lock(TimeDuration const & relative_time)
        {
            return timed_lock(get_system_time()+relative_time);
        }
        bool timed_lock(boost::xtime const & absolute_time)
        {
            return timed_lock(system_time(absolute_time));
        }

#ifdef BOOST_PTHREAD_HAS_TIMEDLOCK
        void lock()
        {
            BOOST_VERIFY(!pthread_mutex_lock(&m));
        }

        void unlock()
        {
            BOOST_VERIFY(!pthread_mutex_unlock(&m));
        }
        
        bool try_lock()
        {
            int const res=pthread_mutex_trylock(&m);
            BOOST_ASSERT(!res || res==EBUSY);
            return !res;
        }
        bool timed_lock(system_time const & abs_time)
        {
            struct timespec const timeout=detail::get_timespec(abs_time);
            int const res=pthread_mutex_timedlock(&m,&timeout);
            BOOST_ASSERT(!res || res==ETIMEDOUT);
            return !res;
        }

        typedef pthread_mutex_t* native_handle_type;
        native_handle_type native_handle()
        {
            return &m;
        }

#else
        void lock()
        {
            boost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            while(is_locked)
            {
                BOOST_VERIFY(!pthread_cond_wait(&cond,&m));
            }
            is_locked=true;
        }

        void unlock()
        {
            boost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            is_locked=false;
            BOOST_VERIFY(!pthread_cond_signal(&cond));
        }
        
        bool try_lock()
        {
            boost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            if(is_locked)
            {
                return false;
            }
            is_locked=true;
            return true;
        }

        bool timed_lock(system_time const & abs_time)
        {
            struct timespec const timeout=detail::get_timespec(abs_time);
            boost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            while(is_locked)
            {
                int const cond_res=pthread_cond_timedwait(&cond,&m,&timeout);
                if(cond_res==ETIMEDOUT)
                {
                    return false;
                }
                BOOST_ASSERT(!cond_res);
            }
            is_locked=true;
            return true;
        }
#endif

        typedef unique_lock<timed_mutex> scoped_timed_lock;
        typedef detail::try_lock_wrapper<timed_mutex> scoped_try_lock;
        typedef scoped_timed_lock scoped_lock;
    };

}

#include <boost/config/abi_suffix.hpp>


#endif
