// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_THREAD_ID_H_
#define OPENAGE_UTIL_THREAD_ID_H_

#include <atomic>

#include "compiler.h"

namespace openage {
namespace util {


/**
 * Designed for usage as a global, thread-local object.
 *
 * The first time it's accessed from a new thread, its constructor will
 * run, assigning a value to 'val'.
 *
 * That value is guaranteed to be unique (and also strictly monotonically
 * rising).
 *
 * See the 'current_thread_id' object below; manual instantiation is
 * discouraged.
 */
class ThreadIdSupplier {
public:
	ThreadIdSupplier();

	const unsigned val;

	// Do not copy/move this thread id; instead, access val.
	ThreadIdSupplier(const ThreadIdSupplier &) = delete;
	ThreadIdSupplier(ThreadIdSupplier &&) = delete;
	ThreadIdSupplier &operator =(const ThreadIdSupplier &) = delete;
	ThreadIdSupplier &operator =(ThreadIdSupplier &&) = delete;
private:
	/**
	 * Used internally to keep track of already-used thread ids.
	 * Holds the id that will be used for the next thread.
	 */
	static std::atomic<unsigned> counting_id;
};


/**
 * Use current_thread_id.val to retreive a unique ID for the current thread.
 */
extern thread_local const ThreadIdSupplier current_thread_id;


}} // namespace openage::util

#endif
