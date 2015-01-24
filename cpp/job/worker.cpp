// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "job_aborted_exception.h"
#include "job_manager.h"
#include "thread_id.h"
#include "worker.h"
#include "../log.h"

namespace openage {
namespace job {

Worker::Worker(JobManager *manager)
	:
	manager{manager},
	is_running{false} {
}

void Worker::start() {
	this->is_running.store(true);
	this->executor.reset(new std::thread{&Worker::process, this});
}

void Worker::stop() {
	this->is_running.store(false);
	this->notify();
}

void Worker::enqueue(std::shared_ptr<JobStateBase> job) {
	std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};
	this->pending_jobs.push(job);
	this->notify();
}

void Worker::notify() {
	this->jobs_available.notify_all();
}

void Worker::join() {
	this->executor->join();
}

void Worker::execute_job(std::shared_ptr<JobStateBase> &job) {
	auto should_abort = [this] () {
		return not this->is_running.load();
	};

	bool aborted = job->execute(should_abort);
	// if the job was not aborted, tell the job manager, that the job has
	// finished
	if (not aborted) {
		this->manager->finish_job(job);
	}
}

void Worker::process() {
	while (this->is_running.load()) {
		// first fetch jobs from the job manager and execute them
		auto global_job = this->manager->fetch_job();
		while (global_job.get() != nullptr) {
			this->execute_job(global_job);
			// check after each execution if the worker thread is still running
			if (not this->is_running.load()) {
				break;
			}

			global_job = this->manager->fetch_job();
		}


		// true if jobs from the local job queue should be fetched
		bool fetch_local = true;
		// true, if the inner loop should be exited
		bool escape = false;

		while (fetch_local) {
			std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};
			while (this->pending_jobs.empty()) {
				// wait for new jobs from the local job queue or from the job
				// manager
				this->jobs_available.wait(lock);
				// check if the worker thread is still running after a wake-up
				if (not this->is_running.load()) {
					escape = true;
					break;
				}

				// check if there is a job from the job manager that has to be
				// executed
				global_job = this->manager->fetch_job();
				if (global_job.get() != nullptr) {
					// if there is a job, execute it and leave the local
					// fetching loop, in order to check for more jobs from the
					// job manager
					lock.unlock();
					this->execute_job(global_job);
					std::unique_lock<std::mutex> other_lock{this->pending_jobs_mutex};
					break;
				}
			}

			// if escape is true, exit the local fetching loop in order to check
			// for jobs from the job manager in the next iteration of the outer
			// loop 
			if (escape) {
				break;
			}

			// fetch a job from the local job queue and execute it
			auto job = this->pending_jobs.front();
			this->pending_jobs.pop();
			// if the local job queue is empty afterwards, exit the local
			// fetching loop and check for  jobs from the job manager in the
			// next iteration of the outer loop
			if (this->pending_jobs.empty()) {
				fetch_local = false;
			}
			lock.unlock();
			this->execute_job(job);
		}
	}
}

}
}
