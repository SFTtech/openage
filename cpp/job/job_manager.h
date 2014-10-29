#ifndef OPENAGE_JOB_JOB_MANAGER_H_
#define OPENAGE_JOB_JOB_MANAGER_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "job.h"
#include "job_state.h"

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

	/** A vector of all worker threads. */
	std::vector<std::thread> workers;

	/** A mutex to synchronize accesses to the internal JobState queue. */
	std::mutex queue_mtx;

	/** A condition variable, whether jobs are currently available or not. */
	std::condition_variable jobs_available;

	/** A queue of JobStates that are to be executed. */
	std::queue<std::shared_ptr<BaseJobState>> pending_jobs;

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
	Job<T> enqueue(std::function<T()> function) {
		auto state = std::make_shared<JobState<T>>(function);

		std::unique_lock<std::mutex> lock{this->queue_mtx};
		this->pending_jobs.push(state);
		this->jobs_available.notify_all();

		return Job<T>{state};
	}

private:
	/**
	 * This function is passed to all worker threads, takes Job's from the
	 * internal queue and executes them.
	 */
	void dispatch_queue();
};

}
}

#endif
