// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_FSLIKEOBJECT_H_
#define OPENAGE_UTIL_FSLIKEOBJECT_H_

#include <iostream>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1
#include "../pyinterface/functional.h"

#include "../pyinterface/pyobject.h"

/*
 * C++ wrappers for openage.util.fslikeobject and openage.util.filelikeobject.
 */

namespace openage {
namespace util {


/**
 * A wrapper for openage.util.fslikeobject.FileSystemLikeObject.
 *
 * Exposes the most important methods, including open(), which
 * returns a stream or FILE-like object.
 *
 * Rather lightweight, as it merely contains a PyObjectRef.
 */
class FileSystemLikeObject : public pyinterface::PyObjectRef {};


}} // openage::util

#endif
