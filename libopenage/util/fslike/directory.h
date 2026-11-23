// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once


#include <string>
#include <sys/stat.h>
#include <tuple>
#include <vector>

#include "fslike.h"


namespace openage {
namespace util {
namespace fslike {


/**
 * Filesystem-like object which uses native libc calls.
 * It is used to directly access your real filesystem
 * that the kernel mounted for you.
 */
class Directory : public FSLike {
public:
	Directory(std::string basepath, bool create_if_missing = false);

	bool is_file(const Path::subpath_t &subpath) override;
	bool is_dir(const Path::subpath_t &subpath) override;
	bool writable(const Path::subpath_t &subpath) override;
	std::vector<Path::path_elem_t> list(const Path::subpath_t &subpath) override;
	bool mkdirs(const Path::subpath_t &subpath) override;
	File open_r(const Path::subpath_t &subpath) override;
	File open_w(const Path::subpath_t &subpath) override;
	File open_rw(const Path::subpath_t &subpath) override;
	File open_a(const Path::subpath_t &subpath) override;
	File open_ar(const Path::subpath_t &subpath) override;
	// inherit the resolve_r/resolve_w functions
	std::string get_native_path(const Path::subpath_t &subpath) override;
	bool rename(const Path::subpath_t &subpath,
	            const Path::subpath_t &target_subpath) override;
	bool rmdir(const Path::subpath_t &subpath) override;
	bool touch(const Path::subpath_t &subpath) override;
	bool unlink(const Path::subpath_t &subpath) override;

	int get_mtime(const Path::subpath_t &subpath) override;
	uint64_t get_filesize(const Path::subpath_t &subpath) override;

	std::ostream &repr(std::ostream &) override;

	static Directory get_temp_directory();

protected:
	/**
	 * resolve the path to an actually usable one.
	 * basically basepath + "/".join(subpath)
	 */
	std::string resolve(const Path::subpath_t &subpath) const;

	std::tuple<struct stat, int> do_stat(const Path::subpath_t &subpath) const;

	std::string basepath;
};
} // namespace fslike
} // namespace util
} // namespace openage
