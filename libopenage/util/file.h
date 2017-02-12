// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>
#include <vector>

#include "dir.h"
#include "filelike/filelike.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObj
#include "../pyinterface/pyobject.h"


// pxd: from libopenage.util.path cimport Path


namespace openage {
namespace util {

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
 *     File() noexcept
 *     File(const string &path, int mode) except +
 *     File(PyObj) except +
 */
class File {
public:
	using seek_t = filelike::FileLike::seek_t;
	using mode_t = filelike::FileLike::mode_t;

	/**
	 * Empty constructor, pls don't use.
	 * It only exists because Cython needs it.
	 */
	File();

	/**
	 * Open a filesystem path, int version.
	 * this is just the integer value of the mode_t, because Cython...
	 */
	File(const std::string &path, int mode);

	/**
	 * Open a filesystem path.
	 */
	File(const std::string &path, mode_t mode=mode_t::R);

	/**
	 * Create a file from an already created filelike.
	 */
	File(std::shared_ptr<filelike::FileLike> filelike);

	/**
	 * Wraps a python filelike object that is already open.
	 * This is called from Cython.
	 *
	 * You will probably never call that manually.
	 */
	File(py::Obj filelike);

	virtual ~File() = default;

	std::string read(ssize_t max=-1);
	bool readable();
	void write(const std::string &data);
	bool writable();
	void seek(ssize_t offset, seek_t how=seek_t::SET);
	bool seekable();
	size_t tell();
	void close();
	void flush();
	ssize_t size();

protected:
	std::shared_ptr<filelike::FileLike> filelike;

	friend std::ostream &operator <<(std::ostream &stream, const File &file);
};

}} // openage::util
