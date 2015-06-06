// This file was taken from <http://libnoise.sourceforge.net/>,
// Copyright 2004-2005 libnoise Authors.
// It's licensed under the terms of the GPL 2.1 or later license.
// Modifications Copyright 2015-2015 the openage authors.
// See copying.md for further legal info.

#ifndef OPENAGE_MAPGEN_PERLINNOISE_LIBNOISE_EXCEPTION_H_
#define OPENAGE_MAPGEN_PERLINNOISE_LIBNOISE_EXCEPTION_H_

namespace noise
{

  /// @addtogroup libnoise
  /// @{

  /// Abstract base class for libnoise exceptions
  class Exception
  {
  };

  /// Invalid parameter exception
  ///
  /// An invalid parameter was passed to a libnoise function or method.
  class ExceptionInvalidParam: public Exception
  {
  };

  /// No module exception
  ///
  /// Could not retrieve a source module from a noise module.
  ///
  /// @note If one or more required source modules were not connected to a
  /// specific noise module, and its GetValue() method was called, that
  /// method will raise a debug assertion instead of this exception.  This
  /// is done for performance reasons.
  class ExceptionNoModule: public Exception
  {
  };

  /// Out of memory exception
  ///
  /// There was not enough memory to perform an action.
  class ExceptionOutOfMemory: public Exception
  {
  };

  /// Unknown exception
  ///
  /// libnoise raised an unknown exception.
  class ExceptionUnknown: public Exception
  {
  };

  /// @}

}

#endif
