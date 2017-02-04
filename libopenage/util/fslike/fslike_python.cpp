// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "fslike_python.h"


namespace openage {
namespace util {


FSLikePython::FSLikePython(py::Obj fsobj)
	:
	fsobj{std::make_shared<py::Obj>(fsobj)} {}


bool FSLikePython::is_file(const Path::parts_t &parts) {
	return pyx_fs_is_file.call(this->fsobj->get_ref(), parts);
}

bool FSLikePython::is_dir(const Path::parts_t &parts) {
	return pyx_fs_is_dir.call(this->fsobj->get_ref(), parts);
}

bool FSLikePython::writable(const Path::parts_t &parts) {
	return pyx_fs_writable.call(this->fsobj->get_ref(), parts);
}

std::vector<Path::part_t> FSLikePython::list(const Path::parts_t &parts) {
	return pyx_fs_list.call(this->fsobj->get_ref(), parts);
}

bool FSLikePython::mkdirs(const Path::parts_t &parts) {
	return pyx_fs_mkdirs.call(this->fsobj->get_ref(), parts);
}

std::shared_ptr<File> FSLikePython::open_r(const Path::parts_t &parts) {
	return pyx_fs_open_r.call(this->fsobj->get_ref(), parts);
}

std::shared_ptr<File> FSLikePython::open_w(const Path::parts_t &parts) {
	return pyx_fs_open_w.call(this->fsobj->get_ref(), parts);
}

bool FSLikePython::rename(const Path::parts_t &parts,
                          const Path::parts_t &target_parts) {
	return pyx_fs_rename.call(this->fsobj->get_ref(), parts, target_parts);
}


bool FSLikePython::rmdir(const Path::parts_t &parts) {
	return pyx_fs_rmdir.call(this->fsobj->get_ref(), parts);
}

bool FSLikePython::touch(const Path::parts_t &parts) {
	return pyx_fs_touch.call(this->fsobj->get_ref(), parts);
}

bool FSLikePython::unlink(const Path::parts_t &parts) {
	return pyx_fs_unlink.call(this->fsobj->get_ref(), parts);
}


int FSLikePython::get_mtime(const Path::parts_t &parts) {
	return pyx_fs_get_mtime.call(this->fsobj->get_ref(), parts);
}

uint64_t FSLikePython::get_filesize(const Path::parts_t &parts) {
	return pyx_fs_get_filesize.call(this->fsobj->get_ref(), parts);
}


std::ostream &FSLikePython::repr(std::ostream &stream) {
	stream << this->fsobj->repr();
	return stream;
}


pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_file;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_is_dir;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_writable;
pyinterface::PyIfFunc<std::vector<std::string>, PyObject *, const std::vector<std::string>&> pyx_fs_list;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_mkdirs;
pyinterface::PyIfFunc<std::shared_ptr<File>, PyObject *, const std::vector<std::string>&> pyx_fs_open_r;
pyinterface::PyIfFunc<std::shared_ptr<File>, PyObject *, const std::vector<std::string>&> pyx_fs_open_w;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&, const std::vector<std::string>&> pyx_fs_rename;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_rmdir;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_touch;
pyinterface::PyIfFunc<bool, PyObject *, const std::vector<std::string>&> pyx_fs_unlink;
pyinterface::PyIfFunc<int, PyObject *, const std::vector<std::string>&> pyx_fs_get_mtime;
pyinterface::PyIfFunc<uint64_t, PyObject *, const std::vector<std::string>&> pyx_fs_get_filesize;

}} // openage::util
