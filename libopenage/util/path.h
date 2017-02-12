// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once


// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>


// pxd: from libopenage.pyinterface.pyobject cimport PyObj
#include "../pyinterface/pyobject.h"


/*
 * C++ wrappers for openage.util.fslikeobject
 */

namespace openage {
namespace util {

class File;

namespace fslike {
class FSLike;
}


/**
 * Analogous to util.fslike.path.Path.
 * For use as constructor argument by RFile and WFile.
 *
 * pxd:
 *
 * cppclass Path:
 *     Path() noexcept
 *     Path(PyObj, const vector[string]&) except +
 *
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

	bool exists();
	bool is_file();
	bool is_dir();
	bool writable();
	std::vector<part_t> list();
	std::vector<Path> iterdir();
	bool mkdirs();
	File open(const std::string &mode="r");
	File open_r();
	File open_w();

	bool rename(const Path &target_path);
	bool rmdir();
	bool touch();
	bool unlink();
	void removerecursive();

	int get_mtime();
	uint64_t get_filesize();

	// TODO: watching of path with inotify or similar
	// int watch(std::function<> callback);
	// void poll_fs_watches(); // call triggered callbacks

	Path get_parent();
	const std::string &get_name();
	std::string get_suffix();
	std::vector<std::string> get_suffixes();
	std::string get_stem();

	Path joinpath(const parts_t &subpaths);
	Path joinpath(const part_t &subpath);
	Path operator [](const parts_t &subpaths);
	Path operator [](const part_t &subpath);
	Path operator /(const part_t &subpath);

	Path with_name(const part_t &name);
	Path with_suffix(const part_t &suffix);

protected:
	std::shared_ptr<fslike::FSLike> fsobj;

	parts_t parts;

	friend std::ostream &operator <<(std::ostream &stream, const Path &path);
};


}} // openage::util
