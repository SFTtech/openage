// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "path.h"

#include "error.h"
#include "fslike/directory.h"
#include "fslike/python.h"

#include "misc.h"
#include "strings.h"


namespace openage {
namespace util {

Path::Path() {}


Path::Path(py::Obj fsobj,
           const std::vector<std::string> &parts) {

	this->parts.reserve(parts.size());

	// normalize the path parts
	for (auto &part : parts) {
		if (part == "." or part == "") {
			continue;
		}
		else if (part == "..") {
			if (this->parts.size() > 0) {
				this->parts.pop_back();
			}
		}
		else {
			this->parts.push_back(part);
		}
	}

	py::Obj result = fslike::pyx_fs_get_native_path.call(fsobj.get_ref(), parts);

	if (result.is(py::None)) {
		// we can't create a c++-variant of the path,
		// so just wrap the python path.
		this->fsobj = std::make_shared<fslike::Python>(fsobj);
	}
	else {
		this->fsobj = std::make_shared<fslike::Directory>(result.bytes());
	}
}


Path::Path(const std::shared_ptr<fslike::FSLike> &fsobj,
           const std::vector<std::string> &parts)
	:
	fsobj{fsobj},
	parts{parts} {}


bool Path::exists() {
	return this->is_file() or this->is_dir();
}

bool Path::is_file() {
	return this->fsobj->is_file(this->parts);
}

bool Path::is_dir() {
	return this->fsobj->is_dir(this->parts);
}

bool Path::writable() {
	return this->fsobj->writable(this->parts);
}

std::vector<Path::part_t> Path::list() {
	return this->fsobj->list(this->parts);
}

std::vector<Path> Path::iterdir() {
	std::vector<Path> ret;

	// return a new path object for each element in that directory
	for (auto &entry : this->fsobj->list(this->parts)) {
		ret.push_back(this->joinpath(entry));
	}

	return ret;
}

bool Path::mkdirs() {
	return this->fsobj->mkdirs(this->parts);
}

File Path::open(const std::string &mode) {
	if (mode == "r") {
		return this->open_r();
	}
	else if (mode == "w") {
		return this->open_w();
	}
	else {
		throw Error{ERR << "unsupported open mode: " << mode};
	}
}

File Path::open_r() {
	return this->fsobj->open_r(this->parts);
}

File Path::open_w() {
	return this->fsobj->open_w(this->parts);
}

bool Path::rename(const Path &target_path) {
	// check if both fsobj pointers are the same
	// maybe a real equality check is better?
	if (this->fsobj != target_path.fsobj) {
		throw Error{
			ERR << "can't rename across two different filesystem like objects"
		};
	}
	return this->fsobj->rename(this->parts, target_path.parts);
}

bool Path::rmdir() {
	return this->fsobj->rmdir(this->parts);
}

bool Path::touch() {
	return this->fsobj->touch(this->parts);
}

bool Path::unlink() {
	return this->fsobj->unlink(this->parts);
}

void Path::removerecursive() {
	if (this->is_dir()) {
		for (auto &path : this->iterdir()) {
			path.removerecursive();
		}
		this->rmdir();
	}
	else {
		this->unlink();
	}
}


int Path::get_mtime() {
	return this->fsobj->get_mtime(this->parts);
}

size_t Path::get_filesize() {
	return this->fsobj->get_filesize(this->parts);
}

// int Path::watch();
// void Path::poll_fs_watches();

Path Path::get_parent() {
	return this->joinpath("..");
}


const std::string &Path::get_name() {
	if (this->parts.size() > 0) {
		return this->parts.back();
	}
	else {
		return util::empty_string;
	}
}

std::string Path::get_suffix() {
	auto &name = this->get_name();
	size_t pos = name.rfind(".");
	if (pos == std::string::npos) {
		return "";
	}

	return name.substr(pos);
}

std::vector<std::string> Path::get_suffixes() {
	std::string name = this->get_name();
	if (name.size() > 0 and name[0] == '.') {
		name = name.substr(1);
	}

	std::vector<std::string> ret;
	// ret = ['.' + suffix for suffix in name.split('.')[1:]]
	return ret;
}

std::string Path::get_stem() {
	const std::string &name = this->get_name();
	size_t pos = name.rfind('.');
	if (pos == std::string::npos) {
		return name;
	}

	return name.substr(0, pos);
}


Path Path::joinpath(const parts_t &subpaths) {
	parts_t new_parts = this->parts;
	for (auto &part : subpaths) {
		new_parts.push_back(part);
	}
	return Path{this->fsobj, new_parts};
}

Path Path::joinpath(const part_t &subpath) {
	return this->joinpath(util::split(subpath, '/'));
}

Path Path::operator [](const parts_t &subpaths) {
	return this->joinpath(subpaths);
}

Path Path::operator [](const part_t &subpath) {
	return this->joinpath(subpath);
}

Path Path::operator /(const part_t &subpath) {
	return this->joinpath(subpath);
}

Path Path::with_name(const part_t &name) {
	return this->get_parent().joinpath(name);
}

Path Path::with_suffix(const part_t &suffix) {
	return this->with_name(this->get_stem() + suffix);
}


std::ostream &operator <<(std::ostream &stream, const Path &path) {
	stream << "Path(";
	path.fsobj->repr(stream);
	stream << ":";

	for (auto &part : path.parts) {
		stream << "/" << part;
	}

	stream << ")";

	return stream;
}

}} // openage::util
