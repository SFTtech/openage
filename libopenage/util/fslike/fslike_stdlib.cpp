// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "fslike_stdlib.h"


namespace openage {
namespace util {


FSLikeNative::FSLikeNative(const std::string &basepath)
	:
	basepath{basepath} {}


bool FSLikeNative::is_file(const Path::parts_t &parts) {

}

bool FSLikeNative::is_dir(const Path::parts_t &parts) {

}

bool FSLikeNative::writable(const Path::parts_t &parts) {

}

std::vector<Path::part_t> FSLikeNative::list(const Path::parts_t &parts) {

}

bool FSLikeNative::mkdirs(const Path::parts_t &parts) {

}

std::shared_ptr<File> FSLikeNative::open_r(const Path::parts_t &parts) {

}

std::shared_ptr<File> FSLikeNative::open_w(const Path::parts_t &parts) {

}

bool FSLikeNative::rename(const Path::parts_t &parts,
                          const Path::parts_t &target_parts) {

}


bool FSLikeNative::rmdir(const Path::parts_t &parts) {

}

bool FSLikeNative::touch(const Path::parts_t &parts) {

}

bool FSLikeNative::unlink(const Path::parts_t &parts) {

}


int FSLikeNative::get_mtime(const Path::parts_t &parts) {

}

uint64_t FSLikeNative::get_filesize(const Path::parts_t &parts) {

}


std::ostream &FSLikeNative::repr(std::ostream &stream) {
	stream << this->basepath;
	return stream;
}

}} // openage::util
