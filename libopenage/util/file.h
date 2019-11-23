// Copyright 2013-2019 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.memory cimport shared_ptr
#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
#include <vector>

// pxd: from libopenage.util.filelike.filelike cimport FileLike
#include "filelike/filelike.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObj
#include "../pyinterface/pyobject.h"

// pxd: from libopenage.util.path cimport Path

namespace openage {
namespace util {

class Path;


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
 *
 *     shared_ptr[FileLike] get_fileobj() except +
 */
class OAAPI File {
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
	File(const py::Obj &filelike);

	virtual ~File() = default;

	/**
	 * Read data from the file and return a string.
	 * If max is negative, return the full remaining file.
	 */
	std::string read(ssize_t max=-1);

	/**
	 * Read data from the file into a buffer,
	 * which has to be large enough to fit max bytes.
	 * If max is negative, read the full remaining file,
	 * so buf has to be as big as the remaining file size.
	 *
	 * Returns the number of bytes that were read.
	 */
	size_t read_to(void *buf, ssize_t max=-1);
	bool readable();
	void write(const std::string &data);
	bool writable();
	void seek(ssize_t offset, seek_t how=seek_t::SET);
	bool seekable();
	size_t tell();
	void close();
	void flush();
	ssize_t size();
	std::vector<std::string> get_lines();

	std::shared_ptr<filelike::FileLike> get_fileobj() const;

protected:
	std::shared_ptr<filelike::FileLike> filelike;

	friend std::ostream &operator <<(std::ostream &stream, const File &file);
};

}} // openage::util
