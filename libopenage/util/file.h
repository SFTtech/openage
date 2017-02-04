// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>
#include <vector>

#include "dir.h"
#include "../error/error.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObj
#include "../pyinterface/pyobject.h"


// pxd: from libopenage.util.path cimport Path


namespace openage {
namespace util {

class FileLike;
class Path;


/**
 * Return the size in bytes of a given file name.
 */
ssize_t file_size(const std::string &filename);


/**
 * Return the size in bytes of a filename relative to a directory.
 */
ssize_t file_size(Dir basedir, const std::string &fname);


/**
 * Read the contents of a given filename.
 */
std::string read_whole_file(const std::string &filename);


/**
 * get the lines of a file.
 *
 * returns vector of strings, each entry is one line in the file.
 */
std::vector<std::string> file_get_lines(const std::string &file_name);


/**
 * Generic File implementation, used in our filesystem-like and file-like
 * abtraction system. Can be created from Python :)
 *
 * TODO: maybe inherit from std::iostream so we can use it the c++-way
 *
 * pxd:
 *
 * cppclass File:
 *     File(PyObj) except +
 */
class File {
public:
	/**
	 * Open a filesystem path.
	 */
	File(const std::string &path);

	/**
	 * Wraps a python filelike object that is already open.
	 * This is called from Cython.
	 *
	 * You will probably never call that manually.
	 */
	File(py::Obj filelike);

	virtual ~File() = default;

	std::string read(ssize_t max=-1);

private:
	std::shared_ptr<FileLike> filelike;
};

}} // openage::util
