// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
// pxd: from libcpp.vector cimport vector

// pxd: from libc.stdint cimport uint64_t
#include <cstdint>
#include <iostream>
// pxd: from libcpp.memory cimport shared_ptr
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.utility cimport pair
#include <utility>

// pxd: from libopenage.util.path cimport Path
// pxd: ctypedef vector[string] parts_t
#include "../path.h"
// pxd: from libopenage.util.file cimport File
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
 *
 * pxd:
 * cppclass FSLike:
 *     Path root() except +
 *     bool is_file(const parts_t &parts) except +
 *     bool is_dir(const parts_t &parts) except +
 *     bool writable(const parts_t &parts) except +
 *     parts_t list(const parts_t &parts) except +
 *     bool mkdirs(const parts_t &parts) except +
 *
 *     File open_r(const parts_t &parts) except +
 *     File open_w(const parts_t &parts) except +
 *     pair[bool, Path] resolve_r(const parts_t &parts) except +
 *     pair[bool, Path] resolve_w(const parts_t &parts) except +
 *     string get_native_path(const parts_t &parts) except +
 *
 *     bool rename(const parts_t &parts,
 *                 const parts_t &target_parts) except +
 *     bool rmdir(const parts_t &parts) except +
 *     bool touch(const parts_t &parts) except +
 *     bool unlink(const parts_t &parts) except +
 *
 *     int get_mtime(const parts_t &parts) except +
 *     uint64_t get_filesize(const parts_t &parts) except +
 *
 *     bool is_python_native() noexcept
 *     shared_ptr[FSLike] shared_from_this() except +
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

	virtual bool is_python_native() const noexcept;

	virtual std::ostream &repr(std::ostream &) = 0;
};

}}} // openage::util::fslike
