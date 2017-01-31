// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1, PyIfFunc2
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectRef, PyObjectPtr
#include "../../pyinterface/pyobject.h"

namespace openage {
namespace util {
namespace py {

// pxd: PyIfFunc2[bool, PyObjectPtr, vector[string]] fs_is_file
extern pyinterface::PyIfFunc<bool, PyObject *, std::vector<std::string>&> fs_is_file;
// pxd: PyIfFunc2[bool, PyObjectPtr, vector[string]] fs_is_dir
extern pyinterface::PyIfFunc<bool, PyObject *, std::vector<std::string>&> fs_is_dir;


}}} // openage::util::py
