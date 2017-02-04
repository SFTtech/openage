// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once


#include <string>
#include <vector>

#include "fslike.h"


namespace openage {
namespace util {


/**
 * Filesystem-like object which uses native libc calls.
 * Makes use of the open()/stat() syscalls etcetc directly.
 */
class FSLikeNative : public FSLike {
public:
	FSLikeNative(const std::string &basepath);

	bool is_file(const Path::parts_t &parts) override;
	bool is_dir(const Path::parts_t &parts) override;
	bool writable(const Path::parts_t &parts) override;
	std::vector<Path::part_t> list(const Path::parts_t &parts) override;
	bool mkdirs(const Path::parts_t &parts) override;
	std::shared_ptr<File> open_r(const Path::parts_t &parts) override;
	std::shared_ptr<File> open_w(const Path::parts_t &parts) override;
	bool rename(const Path::parts_t &parts,
	            const Path::parts_t &target_parts) override;
	bool rmdir(const Path::parts_t &parts) override;
	bool touch(const Path::parts_t &parts) override;
	bool unlink(const Path::parts_t &parts) override;

	int get_mtime(const Path::parts_t &parts) override;
	uint64_t get_filesize(const Path::parts_t &parts) override;

	std::ostream &repr(std::ostream &) override;

protected:
	std::string basepath;
};

}} // openage::util
