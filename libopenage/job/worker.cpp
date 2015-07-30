// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "job_aborted_exception.h"
#include "job_manager.h"
#include "worker.h"


namespace openage {
namespace job {


Worker::Worker(JobManager *manager)
	:
	manager{manager},
	is_running{false} {
}


void Worker::start() {
	this->is_running = true;
	this->executor.reset(new std::thread{&Worker::process, this});
}


void Worker::stop() {
	std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};
	this->is_running = false;
	lock.unlock();
	this->notify();
}


void Worker::enqueue(std::shared_ptr<JobStateBase> job) {
	std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};
	this->pending_jobs.push(job);
	lock.unlock();
	this->notify();
}


void Worker::notify() {
	this->jobs_available.notify_all();
}


void Worker::join() {
	this->executor->join();
}


void Worker::execute_job(std::shared_ptr<JobStateBase> &job) {
	auto should_abort = [this]() {
		return not this->is_running;
	};

	bool aborted = job->execute(should_abort);
	// if the job was not aborted, tell the job manager, that the job has
	// finished
	if (not aborted) {
		this->manager->finish_job(job);
	}
}


void Worker::process() {
	// as long as this worker thread is running repeat all steps
	while (true) {
		// lock the local thread queue
		std::unique_lock<std::mutex> lock{this->pending_jobs_mutex};

		// if this thread shall not run any longer, exit the loop
		if (not this->is_running) {
			return;
		}

		// as long as there are no jobs from the local queue or the job manager
		while (this->pending_jobs.empty() and not this->manager->has_job()) {
			// the thread should wait
			this->jobs_available.wait(lock);

			// when the thread is notified, first check if the thread should be
			// stopped
			if (not this->is_running) {
				return;
			}
		}

		// check if there are jobs in the local queue
		if (not this->pending_jobs.empty()) {
			// fetch the job
			auto job = this->pending_jobs.front();
			this->pending_jobs.pop();
			// release the local queue lock
			lock.unlock();
			// and execute the job
			this->execute_job(job);
		} else {
			// otherwise just unlock the local queue
			lock.unlock();
		}

		// after possibly executing a job from the local queue, check again if
		// the thread should still continue running
		if (not this->is_running) {
			return;
		}

		// now try to fetch a job from the job manager
		auto manager_job = this->manager->fetch_job();
		if (manager_job.get() != nullptr) {
			// and execute it
			this->execute_job(manager_job);
		}
	}
}


}} // namespace openage::job
