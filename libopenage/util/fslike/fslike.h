// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../path.h"


namespace openage {
namespace util {


class File;


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
	virtual std::shared_ptr<File> open_r(const Path::parts_t &parts) = 0;
	virtual std::shared_ptr<File> open_w(const Path::parts_t &parts) = 0;
	virtual bool rename(const Path::parts_t &parts,
	                    const Path::parts_t &target_parts) = 0;
	virtual bool rmdir(const Path::parts_t &parts) = 0;
	virtual bool touch(const Path::parts_t &parts) = 0;
	virtual bool unlink(const Path::parts_t &parts) = 0;

	virtual int get_mtime(const Path::parts_t &parts) = 0;
	virtual uint64_t get_filesize(const Path::parts_t &parts) = 0;

	virtual std::ostream &repr(std::ostream &) = 0;
};

}} // openage::util
