#ifndef OPENAGE_ENGINE_JOB_JOB_H_9DFC09169BF34A61868FFFB8178F4C53
#define OPENAGE_ENGINE_JOB_JOB_H_9DFC09169BF34A61868FFFB8178F4C53

#include <cassert>
#include <exception>
#include <memory>

#include "job_state.h"

namespace openage {
namespace job {

// forward declaration of JobManager
class JobManager;

/**
 * A job is a wrapper around a shared job state object and is returned the job
 * manager. It can be used to retrieve the current state of the job and its
 * result.
 */
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
		if (this->state) {
			return this->state->finished.load();
		}
		return false;
	}

	/**
	 * Returns this job's result if the background execution was successful.
	 * Exceptions that have happened will be rethrown.
	 * It must not be called is is_finished() returns true.
	 */
	T get_result() {
		assert(this->state->finished.load());
		if (this->state->exception != nullptr) {
			std::rethrow_exception(this->state->exception);
		} else {
			return std::move(this->state->result);
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
