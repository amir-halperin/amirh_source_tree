#ifndef PRIVATE_ABS_SELECT_FOR_BOOST_HPP
#define PRIVATE_ABS_SELECT_FOR_BOOST_HPP
//    boost atanh.hpp header file

// This is not part of the officail boost, it was made to solve compilation issues
// with the math lib
// authuer: bsade@ecitele.com
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org for updates, documentation, and revision history.

#include <stdlib.h> // for the C version of abs
#include <cmath>    // for the CPP version of abs

namespace boost
{

  namespace math
  {
    namespace detail
    {
      template<bool FP>
        struct impl_abs_value_base
        {
          template<typename T>
          static int get(T value)
          {
            return ::abs((int)value);
          }
        };
        
        template<>
        struct impl_abs_value_base<true>
        {
          template<typename T>
          static double get(T value)
          {
            return std::abs((double)value);
          }
        };
        
        template<typename T>
        struct impl_abs_value : impl_abs_value_base<boost::is_floating_point<T>::value> 
        {
          // no need to add anything here
        }; 
    } // end of namespace detail
  }   // end of namespace math
  
} // end of namespace boost

#endif  // PRIVATE_ABS_SELECT_FOR_BOOST_HPP
