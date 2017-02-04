// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <string>


namespace openage {
namespace util {

/**
 * File-like class that has the standard operations like
 * read, seek, write etc.
 */
class FileLike {
public:
	FileLike();

	virtual ~FileLike() = default;

	virtual std::string read(ssize_t max) = 0;

	/** string representation of the filelike */
	virtual std::ostream &repr(std::ostream &) = 0;
};


}} // openage::util
