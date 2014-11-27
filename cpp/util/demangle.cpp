// Copyright 2014-2014 the openage authors. See copying.md for legal info.
#include "demangle.h"
#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace openage {
namespace util {

std::string demangle(const char* name) {
	int demangle_status_ok = -1;
	std::unique_ptr<char, void(*)(void*)> res {
		abi::__cxa_demangle(name, NULL, NULL, &demangle_status_ok),
		std::free
	};
	if(demangle_status_ok==0) {
		return res.get();	
	}
	else {
		return name;
	}
}

} // namespace openage
} // namespace util

#else

namespace openage {
namespace util {

std::string demangle(const char* name) {
	return name;
}

} // namespace openage
} // namespace util


#endif
