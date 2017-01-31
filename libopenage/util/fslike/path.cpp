// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "path.h"

#include "abstract.h"


namespace openage {
namespace util {

/*
Path::Path(void *fslike,
           const std::vector<std::string> &parts)
	:
	fs{std::make_shared<pyinterface::PyObjectRef>(fslike)},
	parts{parts} {}
*/

Path::Path(pyinterface::PyObjectRef ref,
           const std::vector<std::string> &parts)
	:
	fs{std::make_shared<pyinterface::PyObjectRef>(ref)},
	parts{parts} {}



bool Path::is_file() {
	return py::fs_is_file.call(this->fs->get_ref(), this->parts);
}


bool Path::is_dir() {
	return py::fs_is_dir.call(this->fs->get_ref(), this->parts);
}



std::ostream &operator <<(std::ostream &stream, const Path &path) {

	stream << "Path("
	       << path.fs->repr()
	       << ":";

	for (auto &part : path.parts) {
		stream << "/" << part;
	}

	stream << ")";

	return stream;
}

}} // openage::util
