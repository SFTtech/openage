// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "path.h"

#include "compiler.h"
#include "fslike/directory.h"
#include "fslike/native.h"
#include "fslike/python.h"
#include "misc.h"
#include "strings.h"
#include "../error/error.h"


namespace openage {
namespace util {


/**
 * Strip out ../ etc
 */
void path_normalizer(Path::parts_t &output, const Path::parts_t &input) {
	output.reserve(input.size());

	// normalize the path parts
	for (auto &part : input) {
		if (part == "." or part == "") {
			continue;
		}
		else if (part == "..") {
			if (likely(output.size() > 0)) {
				output.pop_back();
			}
		}
		else {
			output.push_back(part);
		}
	}
}


Path::Path() {}


Path::Path(py::Obj fsobj_in,
           const std::vector<std::string> &parts) {

	path_normalizer(this->parts, parts);

	// optimization: the fsobj from python may be convertible to
	//               a native c++ implementation.
	//               then calls don't need to be relayed any more.

	// test if fsobj is fslike.Directory
	if (fslike::pyx_fs_is_fslike_directory.call(fsobj_in.get_ref())) {
		this->fsobj = std::make_shared<fslike::Directory>(
			fsobj_in.getattr("path").bytes()
		);
	}
	else {
		// we can't create a c++-variant of the path,
		// so just wrap the python path.
		this->fsobj = std::make_shared<fslike::Python>(fsobj_in);
	}
}


Path::Path(const std::shared_ptr<fslike::FSLike> &fsobj,
           const parts_t &parts)
	:
	fsobj{fsobj} {

	path_normalizer(this->parts, parts);
}


bool Path::exists() const {
	return this->is_file() or this->is_dir();
}

bool Path::is_file() const {
	return this->fsobj->is_file(this->parts);
}

bool Path::is_dir() const {
	return this->fsobj->is_dir(this->parts);
}

bool Path::writable() const {
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

File Path::open(const std::string &mode) const {
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


File Path::open_r() const {
	return this->fsobj->open_r(this->parts);
}


File Path::open_w() const {
	return this->fsobj->open_w(this->parts);
}


std::string Path::get_native_path() const {
	return this->fsobj->get_native_path(this->parts);
}


std::string Path::resolve_native_path(const std::string &mode) const {
	if (mode == "r") {
		return this->resolve_native_path_r();
	}
	else if (mode == "w") {
		return this->resolve_native_path_w();
	}
	else {
		throw Error{ERR << "unsupported resolve mode: " << mode};
	}
}


std::string Path::resolve_native_path_r() const {
	auto resolved_path = this->fsobj->resolve_r(this->parts);

	if (resolved_path.first) {
		return resolved_path.second.get_native_path();
	}
	else {
		throw Error{ERR << "failed to locate readable path: " << *this};
	}
}


std::string Path::resolve_native_path_w() const {
	auto resolved_path = this->fsobj->resolve_w(this->parts);

	if (resolved_path.first) {
		return resolved_path.second.get_native_path();
	}
	else {
		throw Error{ERR << "failed to locate writable path: " << *this};
	}
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


int Path::get_mtime() const {
	return this->fsobj->get_mtime(this->parts);
}

uint64_t Path::get_filesize() const {
	return this->fsobj->get_filesize(this->parts);
}

// int Path::watch();
// void Path::poll_fs_watches();

Path Path::get_parent() const {
	return this->joinpath("..");
}


const std::string &Path::get_name() const {
	if (this->parts.size() > 0) {
		return this->parts.back();
	}
	else {
		return util::empty_string;
	}
}

std::string Path::get_suffix() const {
	auto &name = this->get_name();
	size_t pos = name.rfind(".");
	if (pos == std::string::npos) {
		return "";
	}

	return name.substr(pos);
}

std::vector<std::string> Path::get_suffixes() const {
	std::string name = this->get_name();
	if (name.size() > 0 and name[0] == '.') {
		name = name.substr(1);
	}

	std::vector<std::string> ret;
	// ret = ['.' + suffix for suffix in name.split('.')[1:]]
	return ret;
}

std::string Path::get_stem() const {
	const std::string &name = this->get_name();
	size_t pos = name.rfind('.');
	if (pos == std::string::npos) {
		return name;
	}

	return name.substr(0, pos);
}


Path Path::joinpath(const parts_t &subpaths) const {
	parts_t new_parts = this->parts;
	for (auto &part : subpaths) {
		if (likely(part.size() > 0)) {
			new_parts.push_back(part);
		}
	}
	return Path{this->fsobj, new_parts};
}

Path Path::joinpath(const part_t &subpath) const {
	return this->joinpath(util::split(subpath, '/'));
}

Path Path::operator [](const parts_t &subpaths) const {
	return this->joinpath(subpaths);
}

Path Path::operator [](const part_t &subpath) const {
	return this->joinpath(subpath);
}

Path Path::operator /(const part_t &subpath) const {
	return this->joinpath(subpath);
}

Path Path::with_name(const part_t &name) const {
	return this->get_parent().joinpath(name);
}

Path Path::with_suffix(const part_t &suffix) const {
	return this->with_name(this->get_stem() + suffix);
}

bool Path::operator ==(const Path &other) const {
	return (
		this->fsobj == other.fsobj and
		this->parts == other.parts
	);
}

bool Path::operator !=(const Path &other) const {
	return not (*this == other);
}


fslike::FSLike *Path::get_fsobj() const {
	return this->fsobj.get();
}


const Path::parts_t &Path::get_parts() const {
	return this->parts;
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


std::string filename(const std::string &fullpath) {
	size_t rsep_pos = fullpath.rfind(fslike::PATHSEP);
	if (rsep_pos == std::string::npos) {
		return "";
	} else {
		return fullpath.substr(rsep_pos + 1);
	}
}


std::string dirname(const std::string &fullpath) {
	size_t rsep_pos = fullpath.rfind(fslike::PATHSEP);
	if (rsep_pos == std::string::npos) {
		return "";
	} else {
		return fullpath.substr(0, rsep_pos);
	}
}


}} // openage::util
