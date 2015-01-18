// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_H_
#define OPENAGE_JOB_JOB_H_

#include <cassert>
#include <exception>
#include <memory>

#include "typed_job_state_base.h"

namespace openage {
namespace job {

class JobGroup;
class JobManager;

/**
 * A job is a wrapper around a shared job state object and is returned by the
 * job manager. It can be used to retrieve the current state of the job and its
 * result.
 * A job is a lightweight object which only contains a pointer to its internal
 * shared state. Thus it can be copied around without worrying about
 * performance. Further it is not necessary to create or pass pointers to job
 * objects.
 *
 * @param T the job's result type
 */
template<class T>
class Job {
private:
	/** A shared pointer to the job's shared state. */
	std::shared_ptr<TypedJobStateBase<T>> state;

public:
	/**
	 * Creates an empty job object that is not bound to any state. Should only
	 * be used as dummy object.
	 */
	Job() = default;

	/**
	 * Returns whether this job has finished. If this job wrapper has no
	 * assigned background job, true will be returned.
	 */
	bool is_finished() const {
		if (this->state) {
			return this->state->finished.load();
		}
		return true;
	}

	/**
	 * Returns this job's result if the background execution was successful. If
	 * an exception has happened, it will be rethrown. This method must not be
	 * called, if the job's execution has not yet finished.
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
	 * Creates a job with the given shared state. This method may only be called
	 * by the job manager.
	 */
	Job(std::shared_ptr<TypedJobStateBase<T>> state)
			:
			state{state} {
	}

	/*
	 * Job manager and job group have to be friends of job in order to access the
	 * private constructor.
	 */
	friend class JobGroup;
	friend class JobManager;
};

}
}


#endif
