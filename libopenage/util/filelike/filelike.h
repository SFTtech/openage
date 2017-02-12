// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <string>


namespace openage {
namespace util {
namespace filelike {

/**
 * File-like class that has the standard operations like
 * read, seek, write etc.
 */
class FileLike {
public:
	/**
	 * Seek reference point.
	 *
	 * Don't change the numbers, they're used from Cython.
	 */
	enum class seek_t : int {
		SET = 0,  //!< offset from file beginning
		CUR = 1,  //!< offset from current position
		END = 2   //!< offset from file end
	};

	/**
	 * File access mode.
	 *
	 * Sync the numbers with the fslike/cpp.pyx
	 * because Cython can't enum class yet.
	 */
	enum class mode_t : int {
		R = 0,
		W = 1,
		RW = 2
	};

	FileLike();

	virtual ~FileLike() = default;

	virtual std::string read(ssize_t max=-1) = 0;

	virtual bool readable() = 0;

	virtual void write(const std::string &data) = 0;

	virtual bool writable() = 0;

	virtual void seek(ssize_t offset, seek_t how=seek_t::SET) = 0;
	virtual bool seekable() = 0;
	virtual size_t tell() = 0;
	virtual void close() = 0;
	virtual void flush() = 0;
	virtual ssize_t get_size() = 0;

	/** string representation of the filelike */
	virtual std::ostream &repr(std::ostream &) = 0;
};


}}} // openage::util::filelike
