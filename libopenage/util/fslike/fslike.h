// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "../path.h"
#include "../file.h"


namespace openage {
namespace util {


/**
 * Contains the filesystem-like object abstraction for C++.
 * With that, filesystem access can be dispatched for
 * transparent archive access, union mounts and more.
 */
namespace fslike {


/**
 * Base class for possible path implementations.
 */
class FSLike : public std::enable_shared_from_this<FSLike> {
public:
	FSLike();

	virtual ~FSLike() = default;

	virtual Path root();

	virtual bool is_file(const Path::parts_t &parts) = 0;
	virtual bool is_dir(const Path::parts_t &parts) = 0;
	virtual bool writable(const Path::parts_t &parts) = 0;
	virtual Path::parts_t list(const Path::parts_t &parts) = 0;
	virtual bool mkdirs(const Path::parts_t &parts) = 0;
	virtual File open_r(const Path::parts_t &parts) = 0;
	virtual File open_w(const Path::parts_t &parts) = 0;
	virtual std::pair<bool, Path> resolve_r(const Path::parts_t &parts);
	virtual std::pair<bool, Path> resolve_w(const Path::parts_t &parts);
	virtual std::string get_native_path(const Path::parts_t &parts) = 0;
	virtual bool rename(const Path::parts_t &parts,
	                    const Path::parts_t &target_parts) = 0;
	virtual bool rmdir(const Path::parts_t &parts) = 0;
	virtual bool touch(const Path::parts_t &parts) = 0;
	virtual bool unlink(const Path::parts_t &parts) = 0;

	virtual int get_mtime(const Path::parts_t &parts) = 0;
	virtual uint64_t get_filesize(const Path::parts_t &parts) = 0;

	virtual std::ostream &repr(std::ostream &) = 0;
};

}}} // openage::util::fslike
