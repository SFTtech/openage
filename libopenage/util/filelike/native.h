// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "filelike.h"


namespace openage {
namespace util {
namespace filelike {

/**
 * File-like class that uses native stdlib functions to access data.
 */
class Native : public FileLike {
public:
	Native(const std::string &path, mode_t mode=mode_t::R);
	virtual ~Native();

	std::string read(ssize_t max) override;

	bool readable() override;

	void write(const std::string &data) override;

	bool writable() override;

	void seek(ssize_t offset, seek_t how=seek_t::SET) override;
	bool seekable() override;
	size_t tell() override;
	void close() override;
	void flush() override;
	ssize_t get_size() override;

	std::ostream &repr(std::ostream &) override;

protected:
	std::string path;

	mode_t mode;

	std::fstream file;
};

}}} // openage::util::filelike
