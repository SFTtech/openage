// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once


// pxd: from libcpp cimport bool
// pxd: from libc.stdint cimport uint64_t
#include <cstdint>
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1, PyIfFunc2, PyIfFunc3
#include "../../pyinterface/functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObj, PyObjectPtr, PyObjectRef
#include "../../pyinterface/pyobject.h"

// pxd: from libopenage.util.fslike.fslike cimport FSLike
#include "fslike.h"
// pxd: from libopenage.util.path cimport Path
#include "../path.h"
// pxd: from libopenage.util.file cimport File
#include "../file.h"


namespace openage {
namespace util {
namespace fslike {


/**
 * Filesystem-like object that wraps a filesystem-like object from Python.
 * Calls are relayed via Cython, which performs the data conversion.
 *
 * pxd:
 * cppclass Python(FSLike):
 *     PyObj &get_py_fsobj() except +
 */
class Python : public FSLike {
public:
	Python(py::Obj fsobj);

	bool is_file(const Path::parts_t &parts) override;
	bool is_dir(const Path::parts_t &parts) override;
	bool writable(const Path::parts_t &parts) override;
	std::vector<Path::part_t> list(const Path::parts_t &parts) override;
	bool mkdirs(const Path::parts_t &parts) override;
	File open_r(const Path::parts_t &parts) override;
	File open_w(const Path::parts_t &parts) override;
	std::pair<bool, Path> resolve_r(const Path::parts_t &parts) override;
	std::pair<bool, Path> resolve_w(const Path::parts_t &parts) override;
	std::string get_native_path(const Path::parts_t &parts) override;
	bool rename(const Path::parts_t &parts,
	            const Path::parts_t &target_parts) override;
	bool rmdir(const Path::parts_t &parts) override;
	bool touch(const Path::parts_t &parts) override;
	bool unlink(const Path::parts_t &parts) override;

	int get_mtime(const Path::parts_t &parts) override;
	uint64_t get_filesize(const Path::parts_t &parts) override;

	bool is_python_native() const noexcept override;
	OAAPI py::Obj &get_py_fsobj() const;

	std::ostream &repr(std::ostream &) override;

protected:
	std::shared_ptr<py::Obj> fsobj;
};


// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_is_file
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_file;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_is_dir
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_dir;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_writable
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_writable;

// pxd: PyIfFunc2[vector[string], PyObjectPtr, const vector[string]] pyx_fs_list
extern OAAPI pyinterface::PyIfFunc<std::vector<std::string>, PyObject *, const std::vector<std::string>&> pyx_fs_list;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_mkdirs
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_mkdirs;

// pxd: PyIfFunc2[File, PyObjectPtr, const vector[string]] pyx_fs_open_r
extern OAAPI pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_r;

// pxd: PyIfFunc2[File, PyObjectPtr, const vector[string]] pyx_fs_open_w
extern OAAPI pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_w;

// pxd: PyIfFunc2[Path, PyObjectPtr, const vector[string]] pyx_fs_resolve_r
extern OAAPI pyinterface::PyIfFunc<Path, PyObject *, const std::vector<std::string>&> pyx_fs_resolve_r;

// pxd: PyIfFunc2[Path, PyObjectPtr, const vector[string]] pyx_fs_resolve_w
extern OAAPI pyinterface::PyIfFunc<Path, PyObject *, const std::vector<std::string>&> pyx_fs_resolve_w;

// pxd: PyIfFunc2[PyObjectRef, PyObjectPtr, const vector[string]] pyx_fs_get_native_path
extern OAAPI pyinterface::PyIfFunc<py::Obj, PyObject *, const std::vector<std::string>&> pyx_fs_get_native_path;

// pxd: PyIfFunc3[bool, PyObjectPtr, const vector[string], const vector[string]] pyx_fs_rename
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&, const std::vector<std::string>&> pyx_fs_rename;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_rmdir
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_rmdir;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_touch
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_touch;

// pxd: PyIfFunc2[bool, PyObjectPtr, const vector[string]] pyx_fs_unlink
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_unlink;

// pxd: PyIfFunc2[int, PyObjectPtr, const vector[string]] pyx_fs_get_mtime
extern OAAPI pyinterface::PyIfFunc<int, PyObject *, const std::vector<std::string>&> pyx_fs_get_mtime;

// pxd: PyIfFunc2[uint64_t, PyObjectPtr, const vector[string]] pyx_fs_get_filesize
extern OAAPI pyinterface::PyIfFunc<uint64_t, PyObject *, const std::vector<std::string>&> pyx_fs_get_filesize;


// pxd: PyIfFunc1[bool, PyObjectPtr] pyx_fs_is_fslike_directory
extern OAAPI pyinterface::PyIfFunc<bool, PyObject *> pyx_fs_is_fslike_directory;


}}} // openage::util::fslike
