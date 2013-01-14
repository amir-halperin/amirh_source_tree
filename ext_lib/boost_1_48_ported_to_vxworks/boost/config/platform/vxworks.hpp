//  (C) Copyright Dustin Spicuzza 2009. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  vxWorks specific config options:

#include <limits.h>

#define BOOST_PLATFORM "vxWorks"

#define BOOST_NO_CWCHAR
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
#define BOOST_NO_INTRINSIC_WCHAR_T
#endif // BOOST_NO_INTRINSIC_WCHAR_T

#if defined(__GNUC__) && defined(__STRICT_ANSI__)
# define BOOST_NO_INT64_T
#elif defined(__INT_MAX__) && (__INT_MAX__ == 2147483647)
# define BOOST_NO_INT64_T
#endif  // defined(__GNUC__) && defined(__STRICT_ANSI__)
// some global defines
// -D_VXWORKS_COMPATIBILITY_MODE -DBOOST_FILESYSTEM_NARROW_ONLY -DBOOST_NO_STD_WSTRING -DBOOST_IOSTREAMS_NO_WIDE_STREAMS -DBOOST_NO_INTRINSIC_WCHAR_T -DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_NO_STD_LOCALE
#if !defined(__VXWORKS__)
# define __VXWORKS__
#endif
#if !defined(_VXWORKS_COMPATIBILITY_MODE)
# define _VXWORKS_COMPATIBILITY_MODE
#endif
#if !defined(BOOST_FILESYSTEM_NARROW_ONLY)
# define BOOST_FILESYSTEM_NARROW_ONLY
#endif
#if !defined(BOOST_NO_STD_WSTRING)
#define BOOST_NO_STD_WSTRING
#endif
#if !defined(BOOST_IOSTREAMS_NO_WIDE_STREAMS)
# define BOOST_IOSTREAMS_NO_WIDE_STREAMS
#endif
#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
# define BOOST_NO_INTRINSIC_WCHAR_T
#endif 
#if !defined(BOOST_FILESYSTEM_NO_DEPRECATED)
# define BOOST_FILESYSTEM_NO_DEPRECATED
#endif  // BOOST_FILESYSTEM_NO_DEPRECATED
#if !defined(BOOST_NO_STD_LOCALE)
# define BOOST_NO_STD_LOCALE
#endif  // BOOST_NO_STD_LOCALE

#if !defined(BOOST_HAS_UNISTD_H)
# define BOOST_HAS_UNISTD_H
#endif 
#ifndef BOOST_NO_STD_WSTRING
# define BOOST_NO_STD_WSTRING
#endif  // BOOST_NO_STD_WSTRING

# define BOOST_FILESYSTEM_VERSION 2

// these allow posix_features to work, since vxWorks doesn't
// define them itself
#define _POSIX_TIMERS 1
#define _POSIX_THREADS 1

// vxworks doesn't work with asio serial ports
#if !defined(BOOST_ASIO_DISABLE_SERIAL_PORT)
# define BOOST_ASIO_DISABLE_SERIAL_PORT
#endif  // BOOST_ASIO_DISABLE_SERIAL_PORT

// boilerplate code:
#include <boost/config/posix_features.hpp>
 
