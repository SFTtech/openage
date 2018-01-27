// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <limits>

namespace openage {
namespace math {

constexpr double E            = 2.71828182845904523536;  //!< e
constexpr double LOG2E        = 1.44269504088896340736;  //!< log_2 e
constexpr double LOG10E       = 0.434294481903251827651; //!< log_10 e
constexpr double LN2          = 0.693147180559945309417; //!< log_e 2
constexpr double LN10         = 2.30258509299404568402;  //!< log_e 10
constexpr double PI           = 3.14159265358979323846;  //!< pi
constexpr double PI_2         = 1.57079632679489661923;  //!< pi/2
constexpr double PI_4         = 0.785398163397448309616; //!< pi/4
constexpr double INV_PI       = 0.318309886183790671538; //!< 1/pi
constexpr double INV2_PI      = 0.636619772367581343076; //!< 2/pi
constexpr double INV2_SQRT_PI = 1.12837916709551257390;  //!< 2/sqrt(pi)
constexpr double TAU          = 6.28318530717958647693;  //!< 2*pi
constexpr double DEGSPERRAD   = 0.017453292519943295769; //!< tau/360
constexpr double RADSPERDEG   = 57.29577951308232087679; //!< 360/tau
constexpr double SQRT_2       = 1.41421356237309504880;  //!< sqrt(2)
constexpr double INV_SQRT_2   = 0.707106781186547524401; //!< 1/sqrt(2)

constexpr unsigned int UINT_INF = std::numeric_limits<unsigned int>::max();
constexpr int INT_INF = std::numeric_limits<int>::max();
constexpr double DOUBLE_INF = std::numeric_limits<double>::max();

}} // openage::math
