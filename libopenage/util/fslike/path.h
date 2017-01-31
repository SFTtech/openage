// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
#include <iostream>
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from cpython.ref cimport PyObject
// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1, PyIfFunc2, PyIfFunc3
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectRef, PyObjectPtr
#include "../../pyinterface/pyobject.h"

/*
 * C++ wrappers for openage.util.fslikeobject
 */

namespace openage {
namespace util {


/**
 * Analogous to util.fslike.path.Path.
 * For use as constructor argument by RFile and WFile.
 * pxd:
 *
 * cppclass Path:
 *     Path(PyObjectRef, const vector[string]&) except +
 *
 *     bool exists() except +
 *     bool is_dir() except +
 *     bool is_file() except +
 */
class Path {
public:
	Path(pyinterface::PyObjectRef fsobj, const std::vector<std::string> &parts);

	bool is_file();
	bool is_dir();

protected:
	std::shared_ptr<pyinterface::PyObjectRef> fs;
	std::vector<std::string> parts;

	friend std::ostream &operator <<(std::ostream &stream, const Path &path);
};

}} // openage::util
