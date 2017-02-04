// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once


// pxd: from libcpp cimport bool
// pxd: from libc.stdint cimport uint64_t
#include <cstdint>
// pxd: from libcpp.memory cimport shared_ptr
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc2, PyIfFunc3
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectPtr
#include "../../pyinterface/pyobject.h"

#include "fslike.h"

// pxd: from libopenage.util.path cimport Path
#include "../path.h"
// pxd: from libopenage.util.file cimport File
#include "../file.h"


namespace openage {
namespace util {


/**
 * Filesystem-like object that wraps a filesystem-like object from Python.
 * Calls are relayed via Cython, which performs the data conversion.
 */
class FSLikePython : public FSLike {
public:
	FSLikePython(py::Obj fsobj);

	bool is_file(const Path::parts_t &parts) override;
	bool is_dir(const Path::parts_t &parts) override;
	bool writable(const Path::parts_t &parts) override;
	std::vector<Path::part_t> list(const Path::parts_t &parts) override;
	bool mkdirs(const Path::parts_t &parts) override;
	std::shared_ptr<File> open_r(const Path::parts_t &parts) override;
	std::shared_ptr<File> open_w(const Path::parts_t &parts) override;
	bool rename(const Path::parts_t &parts,
	            const Path::parts_t &target_parts) override;
	bool rmdir(const Path::parts_t &parts) override;
	bool touch(const Path::parts_t &parts) override;
	bool unlink(const Path::parts_t &parts) override;

	int get_mtime(const Path::parts_t &parts) override;
	uint64_t get_filesize(const Path::parts_t &parts) override;

	std::ostream &repr(std::ostream &) override;

protected:
	std::shared_ptr<py::Obj> fsobj;
};


// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_is_file
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_file;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_is_dir
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_dir;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_writable
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_writable;

// pxd: PyIfFunc2[vector[string], PyObjectPtr, const vector[string]] pyx_fs_list
extern pyinterface::PyIfFunc<std::vector<std::string>, PyObject *, const std::vector<std::string>&> pyx_fs_list;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_mkdirs
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_mkdirs;

// pxd: PyIfFunc2[shared_ptr[File], PyObjectPtr, const vector[string]] pyx_fs_open_r
extern pyinterface::PyIfFunc<std::shared_ptr<File>, PyObject *, const std::vector<std::string>&> pyx_fs_open_r;

// pxd: PyIfFunc2[shared_ptr[File], PyObjectPtr, const vector[string]] pyx_fs_open_w
extern pyinterface::PyIfFunc<std::shared_ptr<File>, PyObject *, const std::vector<std::string>&> pyx_fs_open_w;

// pxd: PyIfFunc3[bool, PyObjectPtr, const vector[string], const vector[string]] pyx_fs_rename
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&, const std::vector<std::string>&> pyx_fs_rename;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_rmdir
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_rmdir;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_touch
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_touch;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_unlink
extern pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_unlink;

// pxd: PyIfFunc2[int, PyObjectPtr, const vector[string]] pyx_fs_get_mtime
extern pyinterface::PyIfFunc<int, PyObject *, const std::vector<std::string>&> pyx_fs_get_mtime;

// pxd: PyIfFunc2[uint64_t, PyObjectPtr, const vector[string]] pyx_fs_get_filesize
extern pyinterface::PyIfFunc<uint64_t, PyObject *, const std::vector<std::string>&> pyx_fs_get_filesize;

}} // openage::util
