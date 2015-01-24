// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "thread_id.h"

namespace openage {
namespace job {

std::atomic_uint ThreadId::counting_id;

ThreadId::ThreadId()
	:
	id{ThreadId::counting_id++} {
}

// For each thread a new thread_local ThreadId object is created and counting_id
// will be incremented in its constructor.
thread_local ThreadId thread_id;

}
}
