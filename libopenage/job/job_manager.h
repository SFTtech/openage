// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "abortable_job_state.h"
#include "job.h"
#include "job_group.h"
#include "job_state.h"
#include "job_state_base.h"
#include "types.h"
#include "worker.h"

namespace openage {
namespace job {

/**
 * A job manager can be used to execute functions within separate worker
 * threads.
 */
class JobManager {
private:
	/** The number of internal worker threads. */
	int number_of_workers;

	/**
	 * The index of the worker thread, that is used for the next returned job
	 * group.
	 */
	int group_index;

	/** A vector of all worker threads. */
	std::vector<std::unique_ptr<Worker>> workers;

	/** A mutex to synchronize accesses to the internal job queue. */
	std::mutex pending_jobs_mutex;

	/** A queue of jobs that are to be executed. */
	std::queue<std::shared_ptr<JobStateBase>> pending_jobs;

	/** A mutex to synchronize the finished job map. */
	std::mutex finished_jobs_mutex;

	/**
	 * Mapping from thread id's to a list of jobs, that have been created by the
	 * corresponding thread and have finished.
	 */
	std::unordered_map<size_t, std::vector<std::shared_ptr<JobStateBase>>> finished_jobs;

	/** Whether the job manager is currently running. */
	std::atomic_bool is_running;

public:
	/** Create a new job manager with a specified number of worker threads. */
	JobManager(int number_of_workers);

	/** Destructor that stops the job manager if it is still running. */
	~JobManager();

	JobManager(const JobManager&) = delete;
	JobManager(JobManager&&) = delete;

	JobManager &operator=(const JobManager&) = delete;
	JobManager &operator=(JobManager&&) = delete;

	/** Start the job manager's worker threads. */
	void start();

	/**
	 * Stop the job manager's worker threads. This method blocks until all
	 * currently working threads have finished.
	 */
	void stop();

	/**
	 * Enqueues the given function into the job manager's queue, so that it will
	 * be dispatched by one of the worker threads. A lightweight Job object is
	 * returned, that allows to keep track of the job's state.
	 *
	 * @param function the function that is executed as background job
	 * @param callback the callback function that is executed, when the background
	 *        job has finished
	 */
	template<class T>
	Job<T> enqueue(job_function_t<T> function,
	               callback_function_t<T> callback={}) {
		auto state = std::make_shared<JobState<T>>(function, callback);
		this->enqueue_state(state);
		return Job<T>{state};
	}

	/**
	 * Enqueues the given function into the job manager's queue, so that it will
	 * be dispatched by one of the worker threads. A lightweight job object is
	 * returned, that allows to keep track of the job's state. The passed
	 * function must accept a function object that returns, whether the job
	 * should be aborted at any time. Further it must accept a function that
	 * can be used to abort the execution of the function.
	 *
	 * @param function the function that is executed as background job
	 * @param callback the callback function that is executed, when the background
	 *        job has finished
	 */
	template<class T>
	Job<T> enqueue(abortable_function_t<T> function,
	               callback_function_t<T> callback={}) {
		auto state = std::make_shared<AbortableJobState<T>>(function, callback);
		this->enqueue_state(state);
		return Job<T>{state};
	}

	/**
	 * Creates a job group, in order to be able to execute multiple jobs on the
	 * same worker thread.
	 */
	JobGroup create_job_group();

	/**
	 * Executes callbacks for all job's, that were created by the current thread
	 * and have finished.
	 */
	void execute_callbacks();

private:
	/** Enqueues the given job into the internal job queue. */
	void enqueue_state(std::shared_ptr<JobStateBase> state);

	/**
	 * Returns a job from the internal job queue. If the queue is empty, a
	 * nullptr is returned.
	 */
	std::shared_ptr<JobStateBase> fetch_job();

	/** Returns whether there are jobs to be executed. */
	bool has_job();

	/** Adds a finished job to the internal finished job map. */
	void finish_job(std::shared_ptr<JobStateBase> job);

	/**
	 * A worker has to be a friend of the job manager in order to call the
	 * private finish_job method.
	 */
	friend class Worker;
};

}
}
