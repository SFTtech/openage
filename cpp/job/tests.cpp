// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "job_manager.h"
#include "../log/log.h"

#include <ctime>
#include <chrono>

namespace openage {
namespace job {
namespace tests {


int test_simple_job() {
	JobManager manager{4};
	manager.start();

	bool finished = false;
	bool result = -1;

	auto job_function = []() -> int {
		return 42;
	};
	auto job_callback = [&](result_function_t<int> get_result) {
		int job_result = get_result();
		if (job_result == 42) {
			result = 0;
		}
		finished = true;

	};
	manager.enqueue<int>(job_function, job_callback);

	while (not finished) {
		manager.execute_callbacks();
	}

	manager.stop();
	return result;
}


int test_simple_job_with_exception() {
	JobManager manager{4};
	manager.start();

	bool finished = false;
	bool result = -1;

	auto job_function = []() -> int {
		throw "error string";
	};
	auto job_callback = [&](result_function_t<int> get_result) {
		try {
			get_result();
		} catch (const char *e) {
			result = 0;
		}
		finished = true;

	};
	manager.enqueue<int>(job_function, job_callback);

	while (not finished) {
		manager.execute_callbacks();
	}

	manager.stop();
	return result;

}


/**
 * Compares the time required to process a basic operation
 * between a 4 workers job manager and a 1 worker job manager.
 */
int compare_two_job_managers () {
	JobManager solitary{1};
	JobManager team{4};
	std::atomic<int> goal(100);
	std::atomic<int> done(0);
	std::chrono::high_resolution_clock::time_point begin_time;
	std::chrono::high_resolution_clock::time_point end_time;
	std::chrono::duration<double> solitary_time;
	std::chrono::duration<double> team_time;

	auto job_function = []() -> int {
		return 42;
	};
	auto job_callback = [&](result_function_t<int> get_result) {
		if (get_result() == 42) {
			done++;
		}
	};

	for (int i = 0; i < goal.load(); i++) {
		solitary.enqueue<int>(job_function, job_callback);
		team.enqueue<int>(job_function, job_callback);
	}

	solitary.start();
	team.start();

	// Measures the solitary's job callbacks time execution
	begin_time = std::chrono::high_resolution_clock::now();
	while (done.load() < goal.load()) {
		solitary.execute_callbacks();
	}
	solitary.stop();
	end_time = std::chrono::high_resolution_clock::now();
	solitary_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - begin_time);

	// Measures the team's job callbacks time execution
	begin_time = std::chrono::high_resolution_clock::now();
	while (done.load() < goal.load()) {
		team.execute_callbacks();
	}
	solitary.stop();
	end_time = std::chrono::high_resolution_clock::now();
	team_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - begin_time);

	return (team_time.count() < solitary_time.count() ? 0 : -1);
}


void test_job_manager() {
	int ret;
	const char *testname;
	if ((ret = test_simple_job()) == -1) {
		testname = "test_simple_job";
		goto out;
	} else if ((ret = test_simple_job_with_exception()) == -1) {
		testname = "test_simple_job_with_exception";
		goto out;
	} else if ((ret = compare_two_job_managers()) == -1) {
		testname = "compare_two_job_managers";
		goto out;
	}
	return;
out:
	log::log(MSG(err) << testname << " failed with return value " << ret);
	throw "job manager tests failed";
}


}}} // namespace openage::job::tests
