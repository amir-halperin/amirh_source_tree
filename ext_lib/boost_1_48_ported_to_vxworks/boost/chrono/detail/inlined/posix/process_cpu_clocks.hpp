//  boost process_cpu_clocks.cpp  -----------------------------------------------------------//

//  Copyright Beman Dawes 1994, 2006, 2008
//  Copyright Vicente J. Botet Escriba 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/chrono for documentation.

//--------------------------------------------------------------------------------------//

#include <boost/chrono/config.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>
#include <boost/assert.hpp>

#include <sys/times.h>
#include <unistd.h>
#include <time.h>  // for clock_gettime

#if defined(__VXWORKS__)
//#warning "we are using vxworks workaround for missing tick features from posix"
#include <sysLib.h>
#include <tickLib.h>
  // taken from the Linux man page (http://linux.die.net/man/2/times)
struct tms 
{
      clock_t tms_utime;  /* user time */
      clock_t tms_stime;  /* system time */
      clock_t tms_cutime; /* user time of children */
      clock_t tms_cstime; /* system time of children */
      
      tms() : tms_utime(0), tms_stime(0), tms_cutime(0), tms_cstime(0)
      { // default
      }
      
      explicit tms(unsigned long val) : tms_utime(val), tms_stime(val), tms_cutime(val), tms_cstime(val)
      {
      }
      
      tms& operator = (unsigned long val)
      {
        tms_utime = val;
        tms_stime = val;
        tms_cutime = val;
        tms_cstime = val;
        return *this;
      }
};  
#endif  // __VXWORKS__

namespace boost 
{ 
namespace chrono 
{
namespace chrono_detail
{

namespace
{
  // the function to get teh tick count under vxworks is not the same
  // as with unix base systems, this is a workaround
#if defined(__VXWORKS__)

// note that in vxworks we don't have 
// at least for now, the distinction between
// user time and process time, we have single
// process and all is run in the kernel
inline clock_t uptime_ticks(tms* tm)
{
 unsigned long value = tickGet(); // we cannot tell if this failed..
 *tm = value;
 return static_cast<clock_t>(value);
}

inline long get_tick_count()
{
  return sysClkRateGet();
}
#else
inline long get_tick_count()
{
  return ::sysconf(_SC_CLK_TCK);
}

inline clock_t uptime_ticks(tms* tm)
{
  return ::times( &tm );
}
#endif  // __VXWORKS__

} // end of local namespace

  inline long tick_factor()        // multiplier to convert ticks
                            //  to nanoseconds; -1 if unknown
  {
    static long factor = 0;
    if (!factor)
    {
      if ((factor = get_tick_count()) <= 0)
      {
        factor = -1;
      }
      else
      {
        BOOST_ASSERT(factor <= 1000000l); // doesn't handle large ticks
        factor = 1000000l / factor;  // compute factor
        if (!factor)
        {
          factor = -1;
        }
      }
    }
    return factor;
  }
}

process_real_cpu_clock::time_point process_real_cpu_clock::now() BOOST_CHRONO_NOEXCEPT
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks(&tm);
    if ( c == clock_t(-1) ) // error
    {
      BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            return time_point(
                    microseconds(c*chrono_detail::tick_factor()));
        }
        else
        {
          BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
        }
    }
    return time_point();
}

#if !defined BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_real_cpu_clock::time_point process_real_cpu_clock::now(
        system::error_code & ec) 
{
    
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
        if (BOOST_CHRONO_IS_THROWS(ec))
        {
            boost::throw_exception(
                    system::system_error( 
                            errno, 
                            BOOST_CHRONO_SYSTEM_CATEGORY, 
                            "chrono::process_real_cpu_clock" ));
        }
        else
        {
            ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            if (!BOOST_CHRONO_IS_THROWS(ec)) 
            {
                ec.clear();
            }
            return time_point(
                microseconds(c*chrono_detail::tick_factor()));
        }
        else
        {
            if (BOOST_CHRONO_IS_THROWS(ec))
            {
                boost::throw_exception(
                        system::system_error( 
                                errno, 
                                BOOST_CHRONO_SYSTEM_CATEGORY, 
                                "chrono::process_real_cpu_clock" ));
            }
            else
            {
                ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
                return time_point();
            }
        }
    }
}
#endif

process_user_cpu_clock::time_point process_user_cpu_clock::now() BOOST_CHRONO_NOEXCEPT
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
          BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            return time_point(
                microseconds((tm.tms_utime + tm.tms_cutime)*chrono_detail::tick_factor()));
        }
        else
        {
          BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
        }
    }
    return time_point();
}

#if !defined BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_user_cpu_clock::time_point process_user_cpu_clock::now(
        system::error_code & ec)
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
        if (BOOST_CHRONO_IS_THROWS(ec))
        {
            boost::throw_exception(
                    system::system_error( 
                            errno, 
                            BOOST_CHRONO_SYSTEM_CATEGORY, 
                            "chrono::process_user_cpu_clock" ));
        }
        else
        {
            ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            if (!BOOST_CHRONO_IS_THROWS(ec)) 
            {
                ec.clear();
            }
            return time_point(
                microseconds((tm.tms_utime + tm.tms_cutime)*chrono_detail::tick_factor()));
        }
        else
        {
            if (BOOST_CHRONO_IS_THROWS(ec))
            {
                boost::throw_exception(
                        system::system_error( 
                                errno, 
                                BOOST_CHRONO_SYSTEM_CATEGORY, 
                                "chrono::process_user_cpu_clock" ));
            }
            else
            {
                ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
                return time_point();
            }
        }
    }
}
#endif

process_system_cpu_clock::time_point process_system_cpu_clock::now() BOOST_CHRONO_NOEXCEPT
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
      BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
      return time_point();
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            return time_point(
                microseconds((tm.tms_stime + tm.tms_cstime)*chrono_detail::tick_factor()));
        }
        else
        {
          BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
          return time_point();
        }
    }
}

#if !defined BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_system_cpu_clock::time_point process_system_cpu_clock::now(
        system::error_code & ec)
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
        if (BOOST_CHRONO_IS_THROWS(ec))
        {
            boost::throw_exception(
                    system::system_error(
                            errno,
                            BOOST_CHRONO_SYSTEM_CATEGORY,
                            "chrono::process_system_cpu_clock" ));
        }
        else
        {
            ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            if (!BOOST_CHRONO_IS_THROWS(ec))
            {
                ec.clear();
            }
            return time_point(
                microseconds((tm.tms_stime + tm.tms_cstime)*chrono_detail::tick_factor()));
        }
        else
        {
            if (BOOST_CHRONO_IS_THROWS(ec))
            {
                boost::throw_exception(
                        system::system_error(
                                errno,
                                BOOST_CHRONO_SYSTEM_CATEGORY,
                                "chrono::process_system_cpu_clock" ));
            }
            else
            {
                ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
                return time_point();
            }
        }
    }
}
#endif

process_cpu_clock::time_point process_cpu_clock::now() BOOST_CHRONO_NOEXCEPT
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
      BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            time_point::rep r(
                    1000*c*chrono_detail::tick_factor(),
                    1000*(tm.tms_utime + tm.tms_cutime)*chrono_detail::tick_factor(),
                    1000*(tm.tms_stime + tm.tms_cstime)*chrono_detail::tick_factor());
            return time_point(duration(r));
        }
        else
        {
          BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
        }
    }
    return time_point();
}

#if !defined BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
process_cpu_clock::time_point process_cpu_clock::now( 
        system::error_code & ec ) 
{
    tms tm;
    clock_t c = chrono_detail::uptime_ticks( &tm );
    if ( c == clock_t(-1) ) // error
    {
        if (BOOST_CHRONO_IS_THROWS(ec))
        {
            boost::throw_exception(
                    system::system_error( 
                            errno, 
                            BOOST_CHRONO_SYSTEM_CATEGORY, 
                            "chrono::process_clock" ));
        }
        else
        {
            ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
            return time_point();
        }
    }
    else
    {
        if ( chrono_detail::tick_factor() != -1 )
        {
            time_point::rep r(
                1000*c*chrono_detail::tick_factor(),
                1000*(tm.tms_utime + tm.tms_cutime)*chrono_detail::tick_factor(),
                1000*(tm.tms_stime + tm.tms_cstime)*chrono_detail::tick_factor());
            return time_point(duration(r));
        }
        else
        {
            if (BOOST_CHRONO_IS_THROWS(ec))
            {
                boost::throw_exception(
                        system::system_error( 
                                errno, 
                                BOOST_CHRONO_SYSTEM_CATEGORY, 
                                "chrono::process_clock" ));
            }
            else
            {
                ec.assign( errno, BOOST_CHRONO_SYSTEM_CATEGORY );
                return time_point();
            }
        }
    }
    
}
#endif

} }
