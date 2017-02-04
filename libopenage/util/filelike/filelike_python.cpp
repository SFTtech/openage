// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "filelike_python.h"


namespace openage {
namespace util {

FileLikePython::FileLikePython(py::Obj fileobj)
	:
	fileobj{std::make_shared<py::Obj>(fileobj)} {}

std::string FileLikePython::read(ssize_t max) {
	return pyx_file_read.call(this->fileobj->get_ref(), max);
}

std::ostream &FileLikePython::repr(std::ostream &stream) {
	// TODO: relay to python.
	return stream;
}

pyinterface::PyIfFunc<std::string, PyObject *, ssize_t> pyx_file_read;


}} // openage::util
