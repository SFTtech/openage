// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "job_manager.h"
#include "thread_id.h"
#include "worker.h"
#include "../log.h"

namespace openage {
namespace job {

Worker::Worker(JobManager *parent_manager)
		:
		parent_manager{parent_manager},
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
	job->execute();
	this->parent_manager->finish_job(job);
}

void Worker::process() {
	while (this->is_running.load()) {
		// first fetch jobs from the job manager and execute them
		auto global_job = this->parent_manager->fetch_job();
		if (global_job.get() != nullptr) {
			this->execute_job(global_job);
			global_job = this->parent_manager->fetch_job();
		}

		bool fetch_local = true;
		bool break_out = false;

		while (fetch_local) {
			std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};
			while (this->pending_jobs.empty()) {
				this->jobs_available.wait(lock);
				if (not this->is_running.load()) {
					return;
				}

				global_job = this->parent_manager->fetch_job();
				if (global_job.get() != nullptr) {
					lock.unlock();
					this->execute_job(global_job);
					break_out = true;
					break;
				}
			}

			if (break_out) {
				break;
			}

			auto job = this->pending_jobs.front();
			this->pending_jobs.pop();
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
