// This file was taken from <http://libnoise.sourceforge.net/>,
// Copyright 2004-2005 libnoise Authors.
// It's licensed under the terms of the GPL 2.1 or later license.
// Modifications Copyright 2015-2015 the openage authors.
// See copying.md for further legal info.

#ifndef OPENAGE_MAPGEN_PERLINNOISE_LIBNOISE_INTERP_H_
#define OPENAGE_MAPGEN_PERLINNOISE_LIBNOISE_INTERP_H_

namespace noise
{

  /// @addtogroup libnoise
  /// @{

  /// Performs cubic interpolation between two values bound between two other
  /// values.
  ///
  /// @param n0 The value before the first value.
  /// @param n1 The first value.
  /// @param n2 The second value.
  /// @param n3 The value after the second value.
  /// @param a The alpha value.
  ///
  /// @returns The interpolated value.
  ///
  /// The alpha value should range from 0.0 to 1.0.  If the alpha value is
  /// 0.0, this function returns @a n1.  If the alpha value is 1.0, this
  /// function returns @a n2.
  inline double CubicInterp (double n0, double n1, double n2, double n3,
    double a)
  {
	  double p = (n3 - n2) - (n0 - n1);
	  double q = (n0 - n1) - p;
	  double r = n2 - n0;
	  double s = n1;
	  return p * a * a * a + q * a * a + r * a + s;
  }

  /// Performs linear interpolation between two values.
  ///
  /// @param n0 The first value.
  /// @param n1 The second value.
  /// @param a The alpha value.
  ///
  /// @returns The interpolated value.
  ///
  /// The alpha value should range from 0.0 to 1.0.  If the alpha value is
  /// 0.0, this function returns @a n0.  If the alpha value is 1.0, this
  /// function returns @a n1.
  inline double LinearInterp (double n0, double n1, double a)
  {
    return ((1.0 - a) * n0) + (a * n1);
  }

  /// Maps a value onto a cubic S-curve.
  ///
  /// @param a The value to map onto a cubic S-curve.
  ///
  /// @returns The mapped value.
  ///
  /// @a a should range from 0.0 to 1.0.
  ///
  /// The derivitive of a cubic S-curve is zero at @a a = 0.0 and @a a =
  /// 1.0
  inline double SCurve3 (double a)
  {
    return (a * a * (3.0 - 2.0 * a));
  }

  /// Maps a value onto a quintic S-curve.
  ///
  /// @param a The value to map onto a quintic S-curve.
  ///
  /// @returns The mapped value.
  ///
  /// @a a should range from 0.0 to 1.0.
  ///
  /// The first derivitive of a quintic S-curve is zero at @a a = 0.0 and
  /// @a a = 1.0
  ///
  /// The second derivitive of a quintic S-curve is zero at @a a = 0.0 and
  /// @a a = 1.0
  inline double SCurve5 (double a)
  {
    double a3 = a * a * a;
    double a4 = a3 * a;
    double a5 = a4 * a;
    return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
  }

  // @}

}

#endif
