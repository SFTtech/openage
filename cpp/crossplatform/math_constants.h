// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CROSSPLATFORM_MATH_CONSTANTS_H_
#define OPENAGE_CROSSPLATFORM_MATH_CONSTANTS_H_

// instruct the windows math header to define the constants.
#define _USE_MATH_DEFINES
#include <cmath>

namespace openage {
namespace math {

constexpr double E            = M_E;        //!< e
constexpr double LOG2E        = M_LOG2E;    //!< log_2 e
constexpr double LOG10E       = M_LOG10E;   //!< log_10 e
constexpr double LN2          = M_LN2;      //!< log_e 2
constexpr double LN10         = M_LN10;     //!< log_e 10
constexpr double PI           = M_PI;       //!< pi
constexpr double PI_2         = M_PI_2;     //!< pi/2
constexpr double PI_4         = M_PI_4;     //!< pi/4
constexpr double INV_PI       = M_1_PI;     //!< 1/pi
constexpr double INV2_PI      = M_2_PI;     //!< 2/pi
constexpr double INV2_SQRT_PI = M_2_SQRTPI; //!< 2/sqrt(pi)
constexpr double SQRT_2       = M_SQRT2;    //!< sqrt(2)
constexpr double INV_SQRT_2   = M_SQRT1_2;  //!< 1/sqrt(2)

} // namespace math
} // namespace openage

#endif
