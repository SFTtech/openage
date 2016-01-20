// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

constexpr const char *PATHSEP = "/";

namespace openage {
namespace util {

/**
 * directory management container class.
 *
 * best for storing working directories.
 */
class Dir {
public:
	Dir(const std::string &basedir);
	~Dir();

	std::string join(const std::string &fname) const;
	Dir append(const std::string &suffix) const;

	std::string basedir;
};


/**
 * get the filename (last part) of a given path
 */
std::string basename(const std::string &fullpath);

/**
 * return the head (dirname) part of a path.
 */
std::string dirname(const std::string &fullpath);

}} // openage::util
