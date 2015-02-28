// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_THREAD_ID_H_
#define OPENAGE_JOB_THREAD_ID_H_

#include <atomic>

namespace openage {
namespace job {

/** A class that encapsulates a global unique thread id. */
class ThreadId {
public:
	/** The thread id. */
	const unsigned id;

	/** Creates a new thread id with an monotonically increasing id. */
	ThreadId();

private:
	/** The global id counter. */
	static std::atomic_uint counting_id;
};

/** A global thread local thread id object to retrieve a unique thread id. */
extern thread_local ThreadId thread_id;

}
}

#endif
