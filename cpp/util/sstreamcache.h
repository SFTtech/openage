// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_SSTREAMCACHE_H_
#define OPENAGE_UTIL_SSTREAMCACHE_H_

#include <sstream>
#include <atomic>
#include <type_traits>

namespace openage {
namespace util {


/**
 * designed for usage by OSStreamPtr.
 */
class CachableOSStream {
public:
	std::ostringstream stream;

	CachableOSStream();

	/**
	 * returns a brand-new(*) cached CachableOSStream object.
	 * Origin varies depending on usage (static internal cache, dynamic alloc)
	 * Pointer must be passed to release() after finishing.
	 *
	 * (*) Disclaimer: Object might not actually be brand-new.
	 */
	static CachableOSStream *acquire();

	/**
	 * resets the stream to a brand-new state, and clears the 'blocked'
	 * flag.
	 *
	 * no-op if cs is nullptr.
	 */
	static void release(CachableOSStream *cs);

private:
	/**
	 * true if all of the following are true:
	 *  - object is part of a larger cache
	 *  - object is currently in use (-> unavailable)
	 */
	std::atomic_flag flag;
};


/**
 * Wraps an output string stream.
 * Designed mainly for usage by the logging/MSG system.
 *
 * When creating a message, an internal string stream is used to accumulate
 * the message string. The entire moviation for this cache is the fact that
 * the std::stringstream constructor is extremely slow because it parses the
 * locale environment variables.
 *
 * Using CachableOSStream::acquire and ::release, streams are constructed
 * only once, then reset and re-used when needed.
 */
class OSStreamPtr {
public:
	CachableOSStream *stream_ptr;

	/**
	 * acquires a CachableOSStream object via CachableOSStream::acquire().
	 */
	OSStreamPtr();

	/**
	 * releases the CachableOSStream object back to the pool/frees it
	 */
	~OSStreamPtr();

	// moves ptr
	OSStreamPtr(OSStreamPtr &&) noexcept;
	OSStreamPtr &operator =(OSStreamPtr &&) noexcept;

	// no copy constructor
	OSStreamPtr(const OSStreamPtr &) = delete;
	OSStreamPtr &operator =(const OSStreamPtr &) = delete;

	/**
	 * returns the string that has been assembled thus far.
	 * does not change the internal state, and may be called any number of times.
	 */
	std::string get() const;
};

}} // namespace openage::util

#endif
