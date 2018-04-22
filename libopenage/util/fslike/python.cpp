// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "python.h"

#include "../../log/log.h"

namespace openage {
namespace util {
namespace fslike {


Python::Python(py::Obj fsobj)
	:
	fsobj{std::make_shared<py::Obj>(fsobj)} {}


bool Python::is_file(const Path::parts_t &parts) {
	return pyx_fs_is_file.call(this->fsobj->get_ref(), parts);
}

bool Python::is_dir(const Path::parts_t &parts) {
	return pyx_fs_is_dir.call(this->fsobj->get_ref(), parts);
}

bool Python::writable(const Path::parts_t &parts) {
	return pyx_fs_writable.call(this->fsobj->get_ref(), parts);
}

std::vector<Path::part_t> Python::list(const Path::parts_t &parts) {
	return pyx_fs_list.call(this->fsobj->get_ref(), parts);
}

bool Python::mkdirs(const Path::parts_t &parts) {
	return pyx_fs_mkdirs.call(this->fsobj->get_ref(), parts);
}

File Python::open_r(const Path::parts_t &parts) {
	return pyx_fs_open_r.call(this->fsobj->get_ref(), parts);
}

File Python::open_w(const Path::parts_t &parts) {
	return pyx_fs_open_w.call(this->fsobj->get_ref(), parts);
}

File Python::open_rw(const Path::parts_t &parts) {
	return pyx_fs_open_rw.call(this->fsobj->get_ref(), parts);
}

File Python::open_a(const Path::parts_t &parts) {
	return pyx_fs_open_a.call(this->fsobj->get_ref(), parts);
}

File Python::open_ar(const Path::parts_t &parts) {
	return pyx_fs_open_ar.call(this->fsobj->get_ref(), parts);
}

std::pair<bool, Path> Python::resolve_r(const Path::parts_t &parts) {
	auto path = pyx_fs_resolve_r.call(this->fsobj->get_ref(), parts);
	return std::make_pair(path.get_fsobj() != nullptr, path);
}

std::pair<bool, Path> Python::resolve_w(const Path::parts_t &parts) {
	auto path = pyx_fs_resolve_w.call(this->fsobj->get_ref(), parts);
	return std::make_pair(path.get_fsobj() != nullptr, path);
}

std::string Python::get_native_path(const Path::parts_t &parts) {
	py::Obj ret = pyx_fs_get_native_path.call(this->fsobj->get_ref(), parts);
	if (ret.is(py::None)) {
		return "";
	}
	else {
		return ret.bytes();
	}
}

bool Python::rename(const Path::parts_t &parts,
                    const Path::parts_t &target_parts) {
	return pyx_fs_rename.call(this->fsobj->get_ref(), parts, target_parts);
}


bool Python::rmdir(const Path::parts_t &parts) {
	return pyx_fs_rmdir.call(this->fsobj->get_ref(), parts);
}

bool Python::touch(const Path::parts_t &parts) {
	return pyx_fs_touch.call(this->fsobj->get_ref(), parts);
}

bool Python::unlink(const Path::parts_t &parts) {
	return pyx_fs_unlink.call(this->fsobj->get_ref(), parts);
}


int Python::get_mtime(const Path::parts_t &parts) {
	return pyx_fs_get_mtime.call(this->fsobj->get_ref(), parts);
}

uint64_t Python::get_filesize(const Path::parts_t &parts) {
	return pyx_fs_get_filesize.call(this->fsobj->get_ref(), parts);
}


bool Python::is_python_native() const noexcept {
	return true;
}


py::Obj &Python::get_py_fsobj() const {
	return *this->fsobj.get();
}


std::ostream &Python::repr(std::ostream &stream) {
	stream << this->fsobj->str();
	return stream;
}


pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_file;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_dir;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_writable;
pyinterface::PyIfFunc<std::vector<std::string>, PyObject *, const std::vector<std::string>&> pyx_fs_list;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_mkdirs;
pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_r;
pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_w;
pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_rw;
pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_a;
pyinterface::PyIfFunc<File, PyObject *, const std::vector<std::string>&> pyx_fs_open_ar;
pyinterface::PyIfFunc<Path, PyObject *, const std::vector<std::string>&> pyx_fs_resolve_r;
pyinterface::PyIfFunc<Path, PyObject *, const std::vector<std::string>&> pyx_fs_resolve_w;

pyinterface::PyIfFunc<py::Obj, PyObject *, const std::vector<std::string>&> pyx_fs_get_native_path;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&, const std::vector<std::string>&> pyx_fs_rename;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_rmdir;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_touch;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_unlink;
pyinterface::PyIfFunc<int, PyObject *, const std::vector<std::string>&> pyx_fs_get_mtime;
pyinterface::PyIfFunc<uint64_t, PyObject *, const std::vector<std::string>&> pyx_fs_get_filesize;
pyinterface::PyIfFunc<bool, PyObject *> pyx_fs_is_fslike_directory;

}}} // openage::util::fslike
