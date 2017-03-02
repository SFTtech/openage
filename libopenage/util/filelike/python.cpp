// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "python.h"

#include <type_traits>


namespace openage {
namespace util {
namespace filelike {

Python::Python(py::Obj fileobj)
	:
	fileobj{std::make_shared<py::Obj>(fileobj)} {}


std::string Python::read(ssize_t max) {
	return pyx_file_read.call(this->fileobj->get_ref(), max);
}


bool Python::readable() {
	return pyx_file_readable.call(this->fileobj->get_ref());
}


void Python::write(const std::string &data) {
	pyx_file_write.call(this->fileobj->get_ref(), data);
}


bool Python::writable() {
	return pyx_file_writable.call(this->fileobj->get_ref());
}


void Python::seek(ssize_t offset, seek_t how) {
	// transform the enum class...
	int how_i = static_cast<std::underlying_type<seek_t>::type>(how);

	pyx_file_seek.call(this->fileobj->get_ref(), offset, how_i);
}


bool Python::seekable() {
	return pyx_file_seekable.call(this->fileobj->get_ref());
}


size_t Python::tell() {
	return pyx_file_tell.call(this->fileobj->get_ref());
}


void Python::close() {
	pyx_file_close.call(this->fileobj->get_ref());
}


void Python::flush() {
	pyx_file_flush.call(this->fileobj->get_ref());
}


ssize_t Python::get_size() {
	return pyx_file_size.call(this->fileobj->get_ref());
}


bool Python::is_python_native() const noexcept {
	return true;
}


std::ostream &Python::repr(std::ostream &stream) {
	stream << this->fileobj->repr();
	return stream;
}


pyinterface::PyIfFunc<std::string, PyObject *, ssize_t> pyx_file_read;
pyinterface::PyIfFunc<bool, PyObject *> pyx_file_readable;
pyinterface::PyIfFunc<void, PyObject *, const std::string &> pyx_file_write;
pyinterface::PyIfFunc<bool, PyObject *> pyx_file_writable;
pyinterface::PyIfFunc<void, PyObject *, ssize_t, int> pyx_file_seek;
pyinterface::PyIfFunc<bool, PyObject *> pyx_file_seekable;
pyinterface::PyIfFunc<size_t, PyObject *> pyx_file_tell;
pyinterface::PyIfFunc<void, PyObject *> pyx_file_close;
pyinterface::PyIfFunc<void, PyObject *> pyx_file_flush;
pyinterface::PyIfFunc<ssize_t, PyObject *> pyx_file_size;


}}} // openage::util::filelike
