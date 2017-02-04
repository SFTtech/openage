// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "filelike_stdlib.h"


namespace openage {
namespace util {

FileLikeNative::FileLikeNative(const std::string &path)
	:
	path{path} {}


std::string FileLikeNative::read(ssize_t max) {
	return "TODO native read function";
}


std::ostream &FileLikeNative::repr(std::ostream &stream) {
	stream << "FileLikeNative(" << this->path  << ")";
	return stream;
}

}} // openage::util
