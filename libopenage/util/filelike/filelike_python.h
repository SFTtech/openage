// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>

#include "filelike.h"

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc2
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectPtr
#include "../../pyinterface/pyobject.h"


namespace openage {
namespace util {

/**
 * File-like class that wraps a python open() file-like object.
 */
class FileLikePython : public FileLike {
public:
	FileLikePython(py::Obj fileobj);

	std::string read(ssize_t max) override;

	std::ostream &repr(std::ostream &) override;

protected:
	std::shared_ptr<py::Obj> fileobj;
};


// now follow python interface functions which are
// used to call to python

// pxd: PyIfFunc2[string, PyObjectPtr, ssize_t] pyx_file_read
extern pyinterface::PyIfFunc<std::string, PyObject *, ssize_t> pyx_file_read;


}} // openage::util
