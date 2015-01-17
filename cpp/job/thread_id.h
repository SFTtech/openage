// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_THREAD_ID_H_
#define OPENAGE_JOB_THREAD_ID_H_

#include <atomic>

namespace openage {
namespace job {

class ThreadId {
public:
	const unsigned id;

	ThreadId();

private:
	static std::atomic_uint counting_id;
};

extern thread_local ThreadId thread_id;

}
}

#endif
