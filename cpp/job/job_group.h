// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_GROUP_H_
#define OPENAGE_JOB_JOB_GROUP_H_

#include <cassert>
#include <memory>

#include "abortable_job_state.h"
#include "job.h"
#include "job_state.h"
#include "types.h"
#include "worker.h"

namespace openage {
namespace job {

class JobManager;

class JobGroup {
private:
	Worker *parent_worker;

public:
	JobGroup();

	template<class T>
	Job<T> enqueue(job_function_t<T> function,
			callback_function_t<T> callback={}) {
		assert(this->parent_worker);
		auto state = std::make_shared<JobState<T>>(function, callback);
		this->parent_worker->enqueue(state);
		return Job<T>{state};
	}

	template<class T>
	Job<T> enqueue(abortable_function_t<T> function,
			callback_function_t<T> callback={}) {
		assert(this->parent_worker);
		auto state = std::make_shared<AbortableJobState<T>>(function, callback);
		this->parent_worker->enqueue(state);
		return Job<T>{state};
	}

private:
	JobGroup(Worker *parent_worker);

	friend class JobManager;
};

}
}

#endif
