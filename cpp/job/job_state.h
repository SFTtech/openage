// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_STATE_H_
#define OPENAGE_JOB_JOB_STATE_H_

#include <functional>

#include "typed_job_state_base.h"
#include "types.h"

namespace openage {
namespace job {

/**
 * A JobState is the internal state of a Job. It keeps track of its execution
 * state and stores the Job's result. Further it keeps track of exceptions that
 * occured during the Job's execution.
 * A JobState is created by a JobManager and passed to the user by a lightweight
 * proxy Job object.
 *
 * @param T the result type of this JobState. This type must have a default
 *		constructor and support move semantics.
 */
template<class T>
class JobState : public TypedJobStateBase<T> {
public:
	/** A function object which is executed by the JobManager. */
	job_function_t<T> function;

	/**
	 * Creates a new JobState with the given function, that is to be executed.
	 */
	JobState(job_function_t<T> function, callback_function_t<T> callback)
			:
			TypedJobStateBase<T>{callback},
			function{function} {
	}

	/** Default destructor. */
	virtual ~JobState() = default;

protected:
	virtual T execute_and_get(should_abort_t /*should_abort*/) {
		return std::move(this->function());
	}
};

}
}

#endif
