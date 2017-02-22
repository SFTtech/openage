// Copyright 2017-2017 the openage authors. See copying.md for legal info.

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
	Directory(const std::string &basepath, bool create_if_missing=false);

	bool is_file(const Path::parts_t &parts) override;
	bool is_dir(const Path::parts_t &parts) override;
	bool writable(const Path::parts_t &parts) override;
	std::vector<Path::part_t> list(const Path::parts_t &parts) override;
	bool mkdirs(const Path::parts_t &parts) override;
	File open_r(const Path::parts_t &parts) override;
	File open_w(const Path::parts_t &parts) override;
	std::string get_native_path(const Path::parts_t &parts) override;
	bool rename(const Path::parts_t &parts,
	            const Path::parts_t &target_parts) override;
	bool rmdir(const Path::parts_t &parts) override;
	bool touch(const Path::parts_t &parts) override;
	bool unlink(const Path::parts_t &parts) override;

	int get_mtime(const Path::parts_t &parts) override;
	uint64_t get_filesize(const Path::parts_t &parts) override;

	std::ostream &repr(std::ostream &) override;

protected:
	/**
	 * resolve the path to an actually usable one.
	 * basically basepath + "/".join(parts)
	 */
	std::string resolve(const Path::parts_t &parts) const;

	std::tuple<struct stat, int> do_stat(const Path::parts_t &parts) const;

	std::string basepath;
};

}}} // openage::util::fslike
