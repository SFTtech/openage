// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "abstract.h"

namespace openage {
namespace util {
namespace py {

pyinterface::PyIfFunc<bool, PyObject *, std::vector<std::string>&> fs_is_file;
pyinterface::PyIfFunc<bool, PyObject *, std::vector<std::string>&> fs_is_dir;

}}} // openage::util::py
