#ifndef _ENGINE_UTIL_DIR_H_
#define _ENGINE_UTIL_DIR_H_

#include <string>

constexpr const char *PATHSEP = "/";

namespace engine {
namespace util {

/**
 * directory management container class.
 *
 * best for storing working directories.
 */
class Dir {
public:
	Dir(std::string basedir);
	~Dir();

	std::string join(std::string fname);
	Dir append(std::string suffix);

	std::string basedir;
};


/**
 * get the filename (last part) of a given path
 */
std::string basename(std::string &fullpath);

/**
 * return the head (dirname) part of a path.
 */
std::string dirname(std::string &fullpath);

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_DIR_H_
