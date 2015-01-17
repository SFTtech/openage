// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_MANAGER_H_
#define OPENAGE_JOB_JOB_MANAGER_H_

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "job.h"
#include "job_group.h"
#include "job_state.h"
#include "job_state_base.h"
#include "worker.h"

namespace openage {
namespace job {

/**
 * A JobManager can be used to execute functions within a separate worker
 * threads.
 */
class JobManager {
private:
	/** The number of internal worker threads. */
	int number_of_workers;
	
	int group_index;

	/** A vector of all worker threads. */
	std::vector<std::unique_ptr<Worker>> workers;

	/** A mutex to synchronize accesses to the internal JobState queue. */
	std::mutex pending_jobs_mutex;

	/** A queue of JobStates that are to be executed. */
	std::queue<std::shared_ptr<JobStateBase>> pending_jobs;

	std::mutex finished_jobs_mutex;

	std::unordered_map<unsigned, std::vector<std::shared_ptr<JobStateBase>>> finished_jobs;

	/** Whether the JobManager is currently running. */
	std::atomic_bool is_running;

public:
	/** Create a new job manager with a specified number of worker threads. */
	JobManager(int number_of_workers);

	/** Default destructor. */
	~JobManager() = default;

	JobManager(const JobManager&) = delete;
	JobManager(JobManager&&) = delete;

	JobManager &operator=(const JobManager&) = delete;
	JobManager &operator=(JobManager&&) = delete;

	/** Start the JobManager's worker threads. */
	void start();

	/**
	 * Stop the JobManagers's worker threads. This method blocks until all
	 * currently working threads have finished.
	 */
	void stop();

	/**
	 * Enqueues the given function into the JobManagers's queue, so that it will
	 * be dispatched by one of the worker threads. A lightweight Job object is
	 * returned, that allows to keep track of the Job's state.
	 */
	template<class T>
	Job<T> enqueue(std::function<T()> function,
			std::function<void(T)> callback={}) {
		auto state = std::make_shared<JobState<T>>(function, callback);

		std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};
		this->pending_jobs.push(state);
		for (auto &worker : this->workers) {
			worker->notify();
		}

		return Job<T>{state};
	}

	JobGroup get_job_group();

	void execute_callbacks();

private:
	std::shared_ptr<JobStateBase> fetch_job();

	void finish_job(std::shared_ptr<JobStateBase> job);

	friend class Worker;
};

}
}

#endif
