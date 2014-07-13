#include "dir.h"

namespace engine {
namespace util {

Dir::Dir(std::string basedir) {
	this->basedir = basedir;
}

Dir::~Dir() {

}

std::string Dir::append(std::string fname) {
	return this->basedir + "/" + fname;
}

Dir Dir::merge(std::string suffix) {
	return Dir{this->append(suffix)};
}



} //namespace util
} //namespace engine
