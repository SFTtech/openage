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

/**
 * A job group is a proxy object that forwards job's to a single worker thread.
 * It can be used the assure that multiple jobs are executed on the same
 * background thread.
 */
class JobGroup {
private:
	/** The parent worker that executes all jobs from this job group. */
	Worker *parent_worker;

public:
	/**
	 * Creates a new empty job group with no parent worker. Should only be used
	 * as dummy object.
	 */
	JobGroup();

	/**
	 * Enqueues the given function into the job group's worker thread. A
	 * lightweight job object is returned, that allows to keep track of the
	 * job's state.
	 *
	 * @param function the function that is executed as background job
	 * @param callback the callback function that is executed, when the background
	 *        job has finished 
	 */
	template<class T>
	Job<T> enqueue(job_function_t<T> function,
	               callback_function_t<T> callback={}) {
		assert(this->parent_worker);
		auto state = std::make_shared<JobState<T>>(function, callback);
		this->parent_worker->enqueue(state);
		return Job<T>{state};
	}

	/**
	 * Enqueues the given function into the job group's worker thread. A
	 * lightweight job object is returned, that allows to keep track of the
	 * job's state.The passed function must accept a function object that
	 * returns, whether the job should be aborted at any time. Further it must
	 * accept a function that can be used to abort the execution of the
	 * function.
	 *
	 * @param function the function that is executed as background job
	 * @param callback the callback function that is executed, when the background
	 *        job has finished 
	 */
	template<class T>
	Job<T> enqueue(abortable_function_t<T> function,
	               callback_function_t<T> callback={}) {
		assert(this->parent_worker);
		auto state = std::make_shared<AbortableJobState<T>>(function, callback);
		this->parent_worker->enqueue(state);
		return Job<T>{state};
	}

private:
	/** Creates a new job group with the given parent worker. */
	JobGroup(Worker *parent_worker);

	/** 
	 * The job manager must be a friend of the worker in order to call the
	 * private constructor.
	 */
	friend class JobManager;
};

}
}

#endif
