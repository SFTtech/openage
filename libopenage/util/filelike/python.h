// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool

#include <iostream>
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>

#include "filelike.h"

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1, PyIfFunc2, PyIfFunc3
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectPtr, PyObj
#include "../../pyinterface/pyobject.h"


namespace openage {
namespace util {
namespace filelike {

/**
 * File-like class that wraps a python open() file-like object.
 *
 * pxd:
 * cppclass Python:
 *     PyObj &get_py_fileobj() except +
 */
class Python : public FileLike {
public:
	Python(py::Obj fileobj);

	std::string read(ssize_t max) override;

	bool readable() override;

	void write(const std::string &data) override;

	bool writable() override;

	void seek(ssize_t offset, seek_t how=seek_t::SET) override;
	bool seekable() override;
	size_t tell() override;
	void close() override;
	void flush() override;
	ssize_t get_size() override;

	bool is_python_native() const noexcept override;
	py::Obj &get_py_fileobj() const;

	std::ostream &repr(std::ostream &) override;

protected:
	std::shared_ptr<py::Obj> fileobj;
};


// now follow python interface functions which are
// used to call to python

// pxd: PyIfFunc2[string, PyObjectPtr, ssize_t] pyx_file_read
extern pyinterface::PyIfFunc<std::string, PyObject *, ssize_t> pyx_file_read;

// pxd: PyIfFunc1[bool, PyObjectPtr] pyx_file_readable
extern pyinterface::PyIfFunc<bool, PyObject *> pyx_file_readable;

// pxd: PyIfFunc2[void, PyObjectPtr, const string&] pyx_file_write
extern pyinterface::PyIfFunc<void, PyObject *, const std::string &> pyx_file_write;

// pxd: PyIfFunc1[bool, PyObjectPtr] pyx_file_writable
extern pyinterface::PyIfFunc<bool, PyObject *> pyx_file_writable;

// pxd: PyIfFunc3[void, PyObjectPtr, ssize_t, int] pyx_file_seek
extern pyinterface::PyIfFunc<void, PyObject *, ssize_t, int> pyx_file_seek;

// pxd: PyIfFunc1[bool, PyObjectPtr] pyx_file_seekable
extern pyinterface::PyIfFunc<bool, PyObject *> pyx_file_seekable;

// pxd: PyIfFunc1[size_t, PyObjectPtr] pyx_file_tell
extern pyinterface::PyIfFunc<size_t, PyObject *> pyx_file_tell;

// pxd: PyIfFunc1[void, PyObjectPtr] pyx_file_close
extern pyinterface::PyIfFunc<void, PyObject *> pyx_file_close;

// pxd: PyIfFunc1[void, PyObjectPtr] pyx_file_flush
extern pyinterface::PyIfFunc<void, PyObject *> pyx_file_flush;

// pxd: PyIfFunc1[ssize_t, PyObjectPtr] pyx_file_size
extern pyinterface::PyIfFunc<ssize_t, PyObject *> pyx_file_size;


}}} // openage::util::filelike
