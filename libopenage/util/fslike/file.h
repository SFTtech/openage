// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from libopenage.util.fslike cimport Path
#include "path.h"
// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectRef
#include "../../pyinterface/pyobject.h"
// pxd: from cpython.ref cimport PyObject

/*
 * C++ wrappers for openage.util.filelikeobject.
 */

namespace openage {
namespace util {


/**
 * Wraps a Python file-like object ('rb').
 *
 * ASDF
 * pxd:
 *
 * cppclass RFile:
 *     RFile(Path &) except +
 */
class RFile : public std::istream {
public:
	RFile(Path &path);

private:
	pyinterface::PyObjectRef obj;
};


/**
 * Wraps a Python file-like object ('wb').
 *
 * ASDF
 * pxd:
 *
 * cppclass RFile:
 *     WFile(Path &) except +
 */
class WFile : public std::ostream {
public:
	WFile(Path &path);

private:
	pyinterface::PyObjectRef obj;
};


}} // openage::util
