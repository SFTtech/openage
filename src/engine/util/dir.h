#ifndef _ENGINE_UTIL_DIR_H_
#define _ENGINE_UTIL_DIR_H_

#include <string>

namespace engine {
namespace util {

class Dir {
public:
	Dir(std::string basedir);
	~Dir();

	std::string append(std::string fname);
	Dir merge(std::string suffix);

	std::string basedir;
};

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_DIR_H_
