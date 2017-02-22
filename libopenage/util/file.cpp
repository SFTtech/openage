// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "file.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "path.h"
#include "filelike/native.h"
#include "filelike/python.h"
#include "../error/error.h"
#include "../log/log.h"


namespace openage {
namespace util {


File::File() {}


// yes. i'm sorry. but cython can't enum class yet.
File::File(const std::string &path, int mode)
	:
	File{path, static_cast<mode_t>(mode)} {}


File::File(const std::string &path, mode_t mode) {
	this->filelike = std::make_shared<filelike::Native>(path, mode);
}


File::File(std::shared_ptr<filelike::FileLike> filelike)
	:
	filelike{filelike} {}


File::File(py::Obj filelike) {
	this->filelike = std::make_shared<filelike::Python>(filelike);
}


std::string File::read(ssize_t max) {
	return this->filelike->read(max);
}


bool File::readable() {
	return this->filelike->readable();
}


void File::write(const std::string &data) {
	this->filelike->write(data);
}


bool File::writable() {
	return this->filelike->writable();
}


void File::seek(ssize_t offset, seek_t how) {
	this->filelike->seek(offset, how);
}


bool File::seekable() {
	return this->filelike->seekable();
}


size_t File::tell() {
	return this->filelike->tell();
}


void File::close() {
	this->filelike->close();
}


void File::flush() {
	this->filelike->flush();
}


ssize_t File::size() {
	return this->filelike->get_size();
}


std::vector<std::string> File::get_lines() {
	// TODO: relay the get_lines to the underlaying filelike
	//       which may do a better job in getting the lines.
	//       instead, we read everything and then split up into lines.
	std::string line;
	std::vector<std::string> result{};
	std::istringstream content{this->read()};

	while (std::getline(content, line)) {
		result.push_back(line);
	}

	return result;
}


std::ostream &operator <<(std::ostream &stream, const File &file) {
	stream << "File(";
	file.filelike->repr(stream);
	stream << ")";

	return stream;
}


}} // openage::util
