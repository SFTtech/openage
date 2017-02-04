// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "filelike.h"


namespace openage {
namespace util {

/**
 * File-like class that uses native stdlib functions to access data.
 */
class FileLikeNative : public FileLike {
public:
	FileLikeNative(const std::string &path);

	std::string read(ssize_t max) override;

	std::ostream &repr(std::ostream &) override;

protected:
	std::string path;
};

}} // openage::util
