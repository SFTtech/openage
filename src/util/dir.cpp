#include "dir.h"

#include "../log.h"

namespace openage {
namespace util {

Dir::Dir(std::string basedir) {
	this->basedir = basedir;
}

Dir::~Dir() {

}

std::string Dir::join(std::string fname) {
	return this->basedir + PATHSEP + fname;
}

Dir Dir::append(std::string suffix) {
	if (suffix.length() == 0) {
		return Dir{this->basedir};
	} else {
		return Dir{this->join(suffix)};
	}
}


std::string basename(std::string &fullpath) {
	size_t rsep_pos = fullpath.rfind(PATHSEP);
	if (rsep_pos == std::string::npos) {
		return "";
	} else {
		return fullpath.substr(rsep_pos + 1);
	}
}


std::string dirname(std::string &fullpath) {
	std::string ret;

	size_t rsep_pos = fullpath.rfind(PATHSEP);
	if (rsep_pos == std::string::npos) {
		ret = "";
	} else {
		ret = fullpath.substr(0, rsep_pos);
	}
	return ret;
}

} //namespace util
} //namespace openage
