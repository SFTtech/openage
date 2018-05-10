// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "job_manager.h"

#include "../log/log.h"
#include "../util/thread_id.h"
#include "worker.h"


namespace openage {
namespace job {


JobManager::JobManager(int number_of_workers)
	:
	number_of_workers{number_of_workers},
	group_index{0},
	is_running{false} {

	for (int i = 0; i < number_of_workers; i++) {
		this->workers.emplace_back(new Worker{this});
	}
}


JobManager::~JobManager() {
	this->stop();
}


void JobManager::start() {
	// if the workers have not been started, start them now
	if (not this->is_running.load()) {
		this->is_running.store(true);
		for (auto &worker : this->workers) {
			worker->start();
		}
		log::log(DBG << "Started JobManager with " << this->number_of_workers << " worker threads");
	}
}


void JobManager::stop() {
	// set is_running to false, notify and join all workers
	if (this->is_running.load()) {
		this->is_running.store(false);
		for (auto &worker : this->workers) {
			worker->stop();
		}
		for (auto &worker : this->workers) {
			worker->join();
		}

		log::log(DBG << "Stopped JobManager with " << this->number_of_workers << " worker threads");
	}
}


void JobManager::execute_callbacks() {
	// run callbacks for finished jobs on this thread id.
	size_t id = util::get_current_thread_id();

	std::unique_lock<std::mutex> lock{this->finished_jobs_mutex};
	auto it = this->finished_jobs.find(id);

	if (it != std::end(this->finished_jobs)) {
		// move the  the finished job list here so we can disable the lock
		std::vector<std::shared_ptr<JobStateBase>> jobs;
		std::swap(jobs, it->second);

		// clear the worker's old finished job list
		it->second.clear();
		lock.unlock();

		for (auto &job : jobs) {
			// the job may throw an exception here
			job->execute_callback();
		}
	}
}


JobGroup JobManager::create_job_group() {
	auto index = this->group_index;
	this->group_index = (this->group_index + 1) % this->number_of_workers;
	return JobGroup{this->workers[index].get()};
}


void JobManager::enqueue_state(std::shared_ptr<JobStateBase> state) {
	std::lock_guard<std::mutex> lock{this->pending_jobs_mutex};
	this->pending_jobs.push(state);
	for (auto &worker : this->workers) {
		worker->notify();
	}
}


std::shared_ptr<JobStateBase> JobManager::fetch_job() {
	std::lock_guard<std::mutex> lock{this->pending_jobs_mutex};
	if (this->pending_jobs.empty()) {
		return std::shared_ptr<JobStateBase>{};
	}

	auto job = this->pending_jobs.front();
	this->pending_jobs.pop();
	return job;
}


bool JobManager::has_job() {
	std::lock_guard<std::mutex> lock(this->pending_jobs_mutex);
	return not this->pending_jobs.empty();
}


void JobManager::finish_job(std::shared_ptr<JobStateBase> job) {
	std::lock_guard<std::mutex> lock{this->finished_jobs_mutex};
	auto it = this->finished_jobs.find(job->get_thread_id());
	// if there hasn't been a finished job for the thread_id, create a new
	// entry
	if (it == std::end(this->finished_jobs)) {
		this->finished_jobs.insert({job->get_thread_id(), {job}});
	// otherwise, we append the job to the existing entry
	} else {
		it->second.push_back(job);
	}
}


}} // namespace openage::job
