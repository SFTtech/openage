// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_STATE_H_
#define OPENAGE_JOB_JOB_STATE_H_

#include <functional>

#include "typed_job_state_base.h"
#include "types.h"

namespace openage {
namespace job {

/**
 * A job state supports simple job's with functions that return a single
 * result. While executing the job, it cannot be aborted safely.
 */
template<class T>
class JobState : public TypedJobStateBase<T> {
public:
	/** A function object which is executed by the JobManager. */
	job_function_t<T> function;

	/** Creates a new JobState with the given function, that is to be executed. */
	JobState(job_function_t<T> function, callback_function_t<T> callback)
		:
		TypedJobStateBase<T>{callback},
		function{function} {
	}

	/** Default destructor. */
	virtual ~JobState() = default;

protected:
	virtual T execute_and_get(should_abort_t /*should_abort*/) {
		return this->function();
	}
};

}
}

#endif
