#ifndef _ENGINE_JOB_JOB_MANAGER_H_
#define _ENGINE_JOB_JOB_MANAGER_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "job.h"
#include "job_state.h"

namespace engine {
namespace job {

class JobManager {
private:
	// number of worker threads
	int number_of_workers;
	// all worker threads
	std::vector<std::thread> workers;

	// the mutex to synchronize the queue
	std::mutex queue_mtx;
	// whether there are jobs available at the moment
	std::condition_variable jobs_available;
	// queue of pending jobs
	std::queue<std::shared_ptr<BaseJobState>> pending_jobs;

	std::atomic_bool is_running;

public:
	/**
	 * Create a new job manager with a specified number of worker threads.
	 */
	JobManager(int number_of_workers);
	~JobManager() = default;

	JobManager(const JobManager&) = delete;
	JobManager(JobManager&&) = delete;

	JobManager &operator=(const JobManager&) = delete;
	JobManager &operator=(JobManager&&) = delete;

	/**
	 * Start the job manager's workers.
	 */
	void start();

	/**
	 * Stop the job manager's workers. This method joins all workers.
	 */
	void stop();

	/**
	 * Enqueues the given function into the job manager's queue, so that it will
	 * be dispatched by one of the worker threads.
	 */
	template<class T>
	Job<T> enqueue(std::function<T()> function) {
		auto state = std::make_shared<JobState<T>>(function);

		std::unique_lock<std::mutex> lock{queue_mtx};
		pending_jobs.push(state);
		jobs_available.notify_all();

		return Job<T>{state};
	}

private:
	// this function will be passed to all worker threads
	void dispatch_queue();
};

}
}

#endif
