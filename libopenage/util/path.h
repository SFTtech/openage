// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

/*
 * C++ wrappers for openage.util.fslikeobject
 */


// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from libopenage.pyinterface.pyobject cimport PyObj
#include "../pyinterface/pyobject.h"
#include "file.h"
#include "fslike/native.h"

// pxd: from libopenage.util.fslike.fslike cimport FSLike

namespace openage {
namespace util {

namespace fslike {
class FSLike;
}


/**
 * C++ pendant to the python util.fslike.path.Path
 *
 * Contains a filesystem-like object and path-parts.
 *
 * pxd:
 * cppclass Path:
 *     ctypedef string part_t
 *     ctypedef vector[string] parts_t
 *
 *     Path() noexcept
 *     Path(PyObj, const vector[string]&) except +
 *
 *     FSLike *get_fsobj() except +
 *     const vector[string] &get_parts() except +
 */
class Path {
public:

	/**
	 * Storage type for a part of a path access.
	 * Basically this is the name of a file or directory.
	 */
	using part_t = std::string;

	/**
	 * Storage type for the filesystem access parts
	 * this is basically a list/of/elements/in/a/path.
	 */
	using parts_t = std::vector<part_t>;

	/**
	 * Nullary constructor, pls don't use.
	 * It only exists because Cython can't RAII.
	 * It should be just friend of cython.
	 */
	Path();

	/**
	 * Construct a path object from a python fslike
	 * object. This is called from Cython.
	 *
	 * You will probably never call that manually.
	 */
	Path(py::Obj fslike,
	     const parts_t &parts={});

	/**
	 * Construct a path from a fslike pointer.
	 */
	Path(const std::shared_ptr<fslike::FSLike> &fslike,
	     const parts_t &parts={});

	virtual ~Path() = default;

public:

	bool exists() const;
	bool is_file() const;
	bool is_dir() const;
	bool writable() const;
	std::vector<part_t> list();
	std::vector<Path> iterdir();
	bool mkdirs();
	File open(const std::string &mode="r") const;
	File open_r() const;
	File open_w() const;

	/**
	 * fetch the minimal path that is actually used for opening
	 * TODO: use std::optional
	 */
	std::pair<bool, Path> resolve(const std::string &mode="r") const;

	std::pair<bool, Path> resolve_r() const;
	std::pair<bool, Path> resolve_w() const;

	/**
	 * Return the native path
	 * (something like /your/folder/with/file)
	 * for this path.
	 * returns emptystring ("") if there is no native path.
	 */
	std::string get_native_path() const;

	/**
	 * Resolve the native path by flattening all underlying
	 * filesystem objects (like unions).
	 * Returns the /native/path/on/disk.
	 * Throws up if there is no native path.
	 */
	std::string resolve_native_path(const std::string &mode="r") const;

	std::string resolve_native_path_r() const;
	std::string resolve_native_path_w() const;

	bool rename(const Path &target_path);
	bool rmdir();
	bool touch();
	bool unlink();
	void removerecursive();

	int get_mtime() const;
	uint64_t get_filesize() const;

	// TODO: watching of path with inotify or similar
	//       this should get a Watcher*, which manages the multiple events
	//       otherwise, each file would require an inotify fd.
	//       => see the AssetManager and move functionality from there.
	// int watch(std::function<> callback);
	// void poll_fs_watches(); // call triggered callbacks

	Path get_parent() const;
	const std::string &get_name() const;
	std::string get_suffix() const;
	std::vector<std::string> get_suffixes() const;
	std::string get_stem() const;

	Path joinpath(const parts_t &subpaths) const;
	Path joinpath(const part_t &subpath) const;
	Path operator [](const parts_t &subpaths) const;
	Path operator [](const part_t &subpath) const;
	Path operator /(const part_t &subpath) const;

	Path with_name(const part_t &name) const;
	Path with_suffix(const part_t &suffix) const;

	bool operator ==(const Path &other) const;
	bool operator !=(const Path &other) const;

	fslike::FSLike *get_fsobj() const;
	const parts_t &get_parts() const;

protected:
	std::shared_ptr<fslike::FSLike> fsobj;

	parts_t parts;

	friend std::ostream &operator <<(std::ostream &stream, const Path &path);
};


// helper functions, needed for some parts of convert/

/**
 * get the filename (last part) of a given path
 */
std::string filename(const std::string &fullpath);

/**
 * return the head (dirname) part of a path.
 */
std::string dirname(const std::string &fullpath);


}} // openage::util
