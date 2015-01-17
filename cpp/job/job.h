// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_H_
#define OPENAGE_JOB_JOB_H_

#include <cassert>
#include <exception>
#include <memory>

#include "job_state.h"

namespace openage {
namespace job {

class JobGroup;
class JobManager;

/**
 * A Job is a wrapper around a shared job state object and is returned by the
 * JobManager. It can be used to retrieve the current state of the Job and its
 * result.
 * A Job is a lightweight object which only contains a pointer to its internal
 * shared state. Thus it can be copied around without worrying about
 * performance. Further it is not necessary to create or pass pointers to Job
 * objects.
 *
 * @param T the type that is returned by the Job
 */
template<class T>
class Job {
private:
	/** A shared pointer to the Job's shared state. */
	std::shared_ptr<JobState<T>> state;

public:
	/* Creates an empty Job object that is not bound to any state. */
	Job() = default;

	/** Returns whether this Job has finished. If this job wrapper has no
	 * assigned background job, true will be returned. */
	bool is_finished() const {
		if (this->state) {
			return this->state->finished.load();
		}
		return true;
	}

	/**
	 * Returns this Job's result if the background execution was successful. If
	 * an exception has happened, it will be rethrown. This method must not be
	 * called, if the Job's execution has not yet finished.
	 */
	T get_result() {
		assert(this->state);
		assert(this->state->finished.load());
		if (this->state->exception != nullptr) {
			std::rethrow_exception(this->state->exception);
		} else {
			return std::move(this->state->result);
		}
	}

private:
	/**
	 * Creates a Job with the given shared state. This method may only be called
	 * by the JobManager.
	 */
	Job(std::shared_ptr<JobState<T>> state)
			:
			state{state} {
	}

	/*
	 * JobManager and JobGroup have to be friends of Job in order to access the
	 * private constructor.
	 */
	friend class JobGroup;
	friend class JobManager;
};

}
}


#endif
