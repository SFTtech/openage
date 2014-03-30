#ifndef _ENGINE_JOB_JOB_H_
#define _ENGINE_JOB_JOB_H_

#include <cassert>
#include <exception>
#include <memory>

#include "job_state.h"

namespace engine {
namespace job {

// forward declaration of JobManager
class JobManager;

template<class T>
class Job {
private:
	std::shared_ptr<JobState<T>> state;

public:
	/*
	 * Creates an empty job object that is not bound to any state.
	 */
	Job() = default;

	/**
	 * Returns whether this job has finished.
	 */
	bool is_finished() const {
		if (state) {
			return state->finished.load();
		}
		return false;
	}

	/**
	 * Returns this job's result if the background execution was successful.
	 * Exceptions that have happened will be rethrown.
	 */
	T get_result() {
		assert(state->finished.load());
		if (state->exception != nullptr) {
			std::rethrow_exception(state->exception);
		} else {
			return std::move(state->result);
		}
	}

private:
	// assign a JobState to this job, this can be only done by the JobManager
	// itself
	Job(std::shared_ptr<JobState<T>> state)
			:
			state{state} {
	}
	
	friend class JobManager;
};

}
}


#endif
