#include "dir.h"

namespace engine {
namespace util {

Dir::Dir(std::string basedir) {
	this->basedir = basedir;
}

Dir::~Dir() {

}

std::string Dir::join(std::string fname) {
	return this->basedir + "/" + fname;
}

Dir Dir::merge(std::string suffix) {
	return Dir{this->append(suffix)};
}


std::string basename(std::string &fullpath) {
	int rsep_pos = fullpath.rfind(PATHSEP);
	return fullpath.substr(rsep_pos + 1);
}


std::string dirname(std::string &fullpath) {
	int rsep_pos = fullpath.rfind(PATHSEP);
	return fullpath.substr(0, rsep_pos);
}

} //namespace util
} //namespace engine
