// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_DIR_H_
#define OPENAGE_UTIL_DIR_H_

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

#endif
