// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#include "directory.h"

// HACK: windows.h defines max and min as macros. This results in compile errors.
#ifdef _WIN32
	// defining `NOMINMAX` disables the definition of those macros.
	#define NOMINMAX
#endif

#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>

#ifdef __APPLE__
	#include <sys/time.h>
#endif
#ifdef _WIN32
	#include <direct.h>
	#include <io.h>
	#include <sys/utime.h>
	// HACK: What the heck? I want the std::filesystem library!
	#define O_NOCTTY 0
	#define O_NONBLOCK 0
	#define W_OK 2
#else // ! _MSC_VER
	#include <unistd.h>
#endif

#include "../file.h"
#include "../filelike/native.h"
#include "../misc.h"
#include "../path.h"
#include "./native.h"


namespace openage::util::fslike {


Directory::Directory(std::string basepath, bool create_if_missing) :
	basepath{std::move(basepath)} {
	if (create_if_missing) {
		this->mkdirs({});
	}
}


// We don't need a resolve_r and resolve_w here!
// If the underlying fslike system is a Directory (i.e. this.)
// then we don't have any overlay possibility!
// -> Always resolve just the real system filename.
std::string Directory::resolve(const Path::subpath_t &subpath) const {
	std::string ret = this->basepath;
	for (auto &part : subpath) {
		ret += PATHSEP + part;
	}
	return ret;
}


// TODO: use std::optional when available
std::tuple<struct stat, int> Directory::do_stat(const Path::subpath_t &subpath) const {
	const std::string path = this->resolve(subpath);
	struct stat buf;
	int result = stat(path.c_str(), &buf);

	return std::make_tuple(buf, result);
}


bool Directory::is_file(const Path::subpath_t &subpath) {
	auto stat_result = this->do_stat(subpath);

	// test for regular file
	if (std::get<1>(stat_result) == 0 and S_ISREG(std::get<0>(stat_result).st_mode)) {
		return true;
	}

	return false;
}


bool Directory::is_dir(const Path::subpath_t &subpath) {
	auto stat_result = this->do_stat(subpath);

	// test for regular file
	if (std::get<1>(stat_result) == 0 and S_ISDIR(std::get<0>(stat_result).st_mode)) {
		return true;
	}

	return false;
}


bool Directory::writable(const Path::subpath_t &subpath) {
	Path::subpath_t subpath_test = subpath;

	// try to find the first existing path-part
	while (not(this->is_dir(subpath_test) or this->is_file(subpath_test))) {
		if (subpath_test.size() == 0) {
			throw Error{ERR << "file not found"};
		}

		subpath_test.pop_back();
	}
	const std::string path = this->resolve(subpath_test);

	return access(path.c_str(), W_OK);
}


std::vector<Path::path_elem_t> Directory::list(const Path::subpath_t &subpath) {
	const std::string path = this->resolve(subpath);
	std::vector<Path::path_elem_t> ret;

	DIR *dir;
	struct dirent *ent;

	dir = opendir(path.c_str());

	if (dir == nullptr) {
		throw Error(ERR << "could not list contents of '" << path << "'");
	}

	// walk over dir contents
	while ((ent = readdir(dir)) != nullptr) {
		ret.emplace_back(ent->d_name);
	}

	closedir(dir);

	return ret;
}


bool Directory::mkdirs(const Path::subpath_t &subpath) {
	Path::subpath_t all_subpath = util::split(this->basepath, PATHSEP);

	vector_extend(all_subpath, subpath);

	std::string dirpath;

	for (auto &part : all_subpath) {
		dirpath += PATHSEP + part;

		struct stat buf;

		// it it exists already, try creating the next one
		if (stat(dirpath.c_str(), &buf) == 0 and S_ISDIR(buf.st_mode)) {
			continue;
		}

		// create the folder, umask will turn it to 755.
#ifdef _MSC_VER
		bool dir_created = _mkdir(dirpath.c_str()) == 0;
#elif __MINGW32__
		bool dir_created = mkdir(dirpath.c_str()) == 0;
#else
		bool dir_created = mkdir(dirpath.c_str(), 0777) == 0;
#endif

		if (not dir_created) {
			return false;
		}
	}
	return true;
}


File Directory::open_r(const Path::subpath_t &subpath) {
	return File{
		std::make_shared<filelike::Native>(this->resolve(subpath),
	                                       filelike::Native::mode_t::R)};
}


File Directory::open_w(const Path::subpath_t &subpath) {
	return File{
		std::make_shared<filelike::Native>(this->resolve(subpath),
	                                       filelike::Native::mode_t::W)};
}


File Directory::open_rw(const Path::subpath_t &subpath) {
	return File{
		std::make_shared<filelike::Native>(this->resolve(subpath),
	                                       filelike::Native::mode_t::RW)};
}


File Directory::open_a(const Path::subpath_t &subpath) {
	return File{
		std::make_shared<filelike::Native>(this->resolve(subpath),
	                                       filelike::Native::mode_t::A)};
}


File Directory::open_ar(const Path::subpath_t &subpath) {
	return File{
		std::make_shared<filelike::Native>(this->resolve(subpath),
	                                       filelike::Native::mode_t::AR)};
}


std::string Directory::get_native_path(const Path::subpath_t &subpath) {
	return this->resolve(subpath);
}


bool Directory::rename(const Path::subpath_t &subpath,
                       const Path::subpath_t &target_subpath) {
	return std::rename(this->resolve(subpath).c_str(),
	                   this->resolve(target_subpath).c_str())
	       == 0;
}


bool Directory::rmdir(const Path::subpath_t &subpath) {
	return ::rmdir(this->resolve(subpath).c_str()) == 0;
}


bool Directory::touch(const Path::subpath_t &subpath) {
	const std::string path = this->resolve(subpath);

	// create the file if missing
	int fd = open(
		path.c_str(),
		O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK,
		0666);

	if (fd < 0) {
		return false;
	}

	close(fd);

	// update the timestamp
#ifdef __APPLE__
	int result = utimes(path.c_str(), nullptr) == 0;
#elif defined _WIN32
	int result = _utime(path.c_str(), nullptr) == 0;
#else
	int result = utimensat(AT_FDCWD, path.c_str(), nullptr, 0) == 0;
#endif

	return result;
}


bool Directory::unlink(const Path::subpath_t &subpath) {
	return std::remove(this->resolve(subpath).c_str()) == 0;
}


int Directory::get_mtime(const Path::subpath_t &subpath) {
	auto stat_result = this->do_stat(subpath);

	// return the mtime
	if (std::get<1>(stat_result) == 0) {
#ifdef __APPLE__
		return std::get<0>(stat_result).st_mtimespec.tv_sec;
#elif defined _WIN32
		return std::get<0>(stat_result).st_mtime;
#else
		return std::get<0>(stat_result).st_mtim.tv_sec;
#endif
	}

	throw Error{ERR << "can't get mtime"};
}


uint64_t Directory::get_filesize(const Path::subpath_t &subpath) {
	auto stat_result = this->do_stat(subpath);

	// return the mtime
	if (std::get<1>(stat_result) == 0) {
		return std::get<0>(stat_result).st_size;
	}

	throw Error{ERR << "can't get filesize"};
}


std::ostream &Directory::repr(std::ostream &stream) {
	stream << this->basepath;
	return stream;
}

Directory Directory::get_temp_directory() {
	std::filesystem::path path = std::filesystem::temp_directory_path() / std::tmpnam(nullptr);
	std::string temp_dir_path = path.string();
	bool create = true;
	Directory directory = Directory(temp_dir_path, create);
	return directory;
}

} // namespace openage::util::fslike
