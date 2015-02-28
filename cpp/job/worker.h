// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_WORKER_H_
#define OPENAGE_JOB_WORKER_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>

#include "job_state_base.h"

namespace openage {
namespace job {

class JobManager;

/**
 * A worker encapsulates the execution of multiple jobs in a single background
 * thread.
 */
class Worker {
private:
	/** The parent job manager, this worker is fetching jobs from. */
	JobManager *manager;

	/** Whether this worker thread is still running. */
	std::atomic_bool is_running;

	/** The executing thread. */
	std::unique_ptr<std::thread> executor;

	/** A mutex to synchronize the internal pending jobs queue. */
	std::mutex pending_jobs_mutex;

	/** A queue of jobs that are to be executed. */
	std::queue<std::shared_ptr<JobStateBase>> pending_jobs;

	/** A condition variable to wait for new jobs. */
	std::condition_variable jobs_available;

public:
	/** Constructs a new worker with the parent job manager. */
	Worker(JobManager *manager);

	/** Default destructor. */
	~Worker() = default;

	/** Starts this worker. */
	void start();

	/** Stops this worker. */
	void stop();

	/** Joins the internal executing thread. */
	void join();

	/** Adds the given job to the internal pending job queue. */
	void enqueue(std::shared_ptr<JobStateBase> job);

	/**
	 * Notifies this worker, that new jobs are available. This method is
	 * although called from the parent job manager.
	 */
	void notify();

private:
	/**
	 * Executes the given job and tells the parent job manager, when it has
	 * finished.
	 */
	void execute_job(std::shared_ptr<JobStateBase> &job);

	/**
	 * Fetches pending jobs from the parent job manager and from the internal
	 * pending job queue and executes them. If no jobs are available the
	 * internal execution thread is wait using a condition variable.
	 */
	void process();
};

}
}

#endif
