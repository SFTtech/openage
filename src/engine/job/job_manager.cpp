#include "job_manager.h"

#include "../log.h"

namespace engine {
namespace job {

JobManager::JobManager(int number_of_workers)
		:
		number_of_workers{number_of_workers},
		is_running{false} {
}

void JobManager::start() {
	// if the workers have not been started, start them now
	if (!is_running.load()) {
		is_running.store(true);
		for (int i = 0; i < number_of_workers; i++) {
			workers.push_back(std::thread{&JobManager::dispatch_queue, this});
		}
	}
	log::msg("Started JobManager with %d worker threads", number_of_workers);
}

void JobManager::stop() {
	// set is_running to false, notify and join all workers
	is_running.store(false);
	for (auto &worker : workers) {
		jobs_available.notify_all();
		worker.join();
	}
	workers.clear();
	log::msg("Stopped JobManager with %d worker threads", number_of_workers);
}

void JobManager::dispatch_queue() {
	// loop as long is_running is set
	while (is_running.load()) {
		// aquire the lock on the queue
		std::unique_lock<std::mutex> lock{queue_mtx};
		while (pending_jobs.empty()) {
			// wait as long as there are new jobs available or the worker should
			// be stopped.
			jobs_available.wait(lock);
			// check whether the thread should be stopped
			if (!is_running.load()) {
				return;
			}
		}

		// fetch a job from the queue
		auto job = pending_jobs.front();
		pending_jobs.pop();
		lock.unlock();

		// and execute it
		job->execute();
	}
}

}
}
