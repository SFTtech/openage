// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "native.h"

#include <sstream>

#include "../../error/error.h"


namespace openage {
namespace util {
namespace filelike {

Native::Native(const std::string &path, mode_t mode)
	:
	path{path},
	mode{mode} {

	std::ios_base::openmode open_mode;

	switch (this->mode) {
	case mode_t::R:
		open_mode = std::ios_base::in;
		break;
	case mode_t::W:
		open_mode = std::ios_base::out;
		break;
	case mode_t::RW:
		open_mode = std::ios_base::in | std::ios_base::out;
		break;
	default:
		throw Error{ERR << "unknown open mode"};
	}

	this->file.open(this->path, open_mode);
}


Native::~Native() {
	this->close();
}


std::string Native::read(ssize_t max) {
	// read whole file:
	if (max < 0) {
		std::string ret;

		// get remaining size and prepare the string size
		// to avoid resizing
		size_t pos = this->file.tellg();
		this->file.seekg(0, std::ios::end);
		ret.reserve(static_cast<size_t>(this->file.tellg()) - pos);
		this->file.seekg(pos, std::ios::beg);

		ret.assign(std::istreambuf_iterator<char>(this->file),
		           std::istreambuf_iterator<char>());

		return ret;
	}
	// read `max` bytes
	else {
		// create string with required size
		std::string ret;
		ret.resize(max);

		// and read the data
		this->file.read(&ret[0], max);
		return ret;
	}
}


bool Native::readable() {
	return (this->mode == mode_t::R or
	        this->mode == mode_t::RW);
}


void Native::write(const std::string &data) {
	this->file.write(data.c_str(), data.size());
}


bool Native::writable() {
	return (this->mode == mode_t::W or
	        this->mode == mode_t::RW);
}


void Native::seek(ssize_t offset, seek_t how) {
	std::ios::seekdir where;

	switch (how) {
	case seek_t::SET:
		where = std::ios::beg;
		break;
	case seek_t::CUR:
		where = std::ios::cur;
		break;
	case seek_t::END:
		where = std::ios::end;
		break;
	default:
		throw Error{ERR << "invalid seek mode"};
	}

	this->file.seekg(offset, where);
}


bool Native::seekable() {
	return true;
}


size_t Native::tell() {
	return this->file.tellg();
}


void Native::close() {
	this->file.close();
}


void Native::flush() {
	this->file.flush();
}


ssize_t Native::get_size() {
	// remember where we were
	size_t pos = this->file.tellg();

	// go to the end
	this->file.seekg(0, std::ios::end);
	ssize_t size = static_cast<ssize_t>(this->file.tellg());

	// return to position
	this->file.seekg(pos, std::ios::beg);

	return size;
}


std::ostream &Native::repr(std::ostream &stream) {
	stream << "Native(" << this->path  << ")";
	return stream;
}

}}} // openage::util::filelike
