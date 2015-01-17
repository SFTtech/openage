// Copyright 2014-2015 the openage authors. See copying.md for legal info.

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

class Worker {
private:
	JobManager *parent_manager;

	std::function<std::shared_ptr<JobStateBase>()> fetch_job;

	std::atomic_bool is_running;

	std::unique_ptr<std::thread> executor;

	std::mutex pending_jobs_mutex;
	std::queue<std::shared_ptr<JobStateBase>> pending_jobs;

	std::condition_variable jobs_available;

public:
	Worker(JobManager *parent_manager);
	~Worker() = default;

	void start();
	void stop();

	void join();

	void enqueue(std::shared_ptr<JobStateBase> job);

	void notify();

private:
	void execute_job(std::shared_ptr<JobStateBase> &job);

	void process();
};

}
}

#endif
