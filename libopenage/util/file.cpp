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

ssize_t file_size(const std::string &filename) {
	struct stat st;

	if (stat(filename.c_str(), &st) < 0) {
		return -1;
	}

	return st.st_size;
}

ssize_t file_size(Dir basedir, const std::string &fname) {
	return file_size(basedir.join(fname));
}


std::string read_whole_file(const std::string &filename) {
	std::ifstream file{filename};

	std::string str{
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};

	return str;
}

std::vector<std::string> file_get_lines(const std::string &file_name) {
	std::string line;
	std::vector<std::string> result{};
	std::ifstream file{file_name};

	while (std::getline(file, line)) {
		result.push_back(line);
	}

	return result;
}


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


std::ostream &operator <<(std::ostream &stream, const File &file) {
	stream << "File(";
	file.filelike->repr(stream);
	stream << ")";

	return stream;
}


}} // openage::util
