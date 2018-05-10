// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "thread_id.h"

#include "../config.h"

#if HAVE_THREAD_LOCAL_STORAGE
#include <atomic>
#else
#include <thread>
#endif

namespace openage {
namespace util {

#if HAVE_THREAD_LOCAL_STORAGE

/**
 * Designed for usage as a global, thread-local object.
 *
 * The first time it's accessed from a new thread, its constructor will
 * run, assigning a value to 'val'.
 */
class ThreadIdSupplier {
public:
	ThreadIdSupplier()
	:
	val{ThreadIdSupplier::counting_id++} {}

	const size_t val;

private:
	/**
	 * Used internally to keep track of already-used thread ids.
	 * Holds the id that will be used for the next thread.
	 */
	static std::atomic<size_t> counting_id;
};

std::atomic<size_t> ThreadIdSupplier::counting_id{0};

#endif

size_t get_current_thread_id() {
	#if HAVE_THREAD_LOCAL_STORAGE
	static thread_local ThreadIdSupplier current_thread_id;
	return current_thread_id.val;
	#else
	return std::hash<std::thread::id>()(std::this_thread::get_id());
	#endif
}

}} // namespace openage::util
