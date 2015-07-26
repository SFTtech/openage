// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "job_manager.h"
#include "../log/log.h"

#include <thread>

namespace openage {
namespace job {
namespace tests {


int test_simple_job() {
	JobManager manager{4};
	manager.start();

	std::atomic<int> finish_count(0);
	int job_count = 10;
	bool result = -1;

	auto job_function = []() -> int {
		return 42;
	};
	auto job_callback = [&](result_function_t<int> get_result) {
		int job_result = get_result();
		if (job_result == 42) {
			result = 0;
		}
		finish_count++;
	};

	for (int i = 0; i < job_count; i++) {
		manager.enqueue<int>(job_function, job_callback);
	}

	while (finish_count.load() < job_count) {
		manager.execute_callbacks();
	}

	manager.stop();
	return result;
}


int test_simple_job_with_exception() {
	JobManager manager{4};
	manager.start();

	int good_jobs = 5;
	int bad_jobs = 5;
	std::atomic<int> finished(0);
	std::atomic<int> errors(0);

	auto exception_job_function = []() -> int {
		throw "error string";
	};

	auto job_function = []() -> int {
		return 42;
	};

	auto job_callback = [&](result_function_t<int> get_result) {
		try {
			get_result();
		} catch (const char *e) {
			errors++;
		}
		finished++;
	};

	for (int i = 0; i < good_jobs; i++) {
		manager.enqueue<int>(job_function, job_callback);
	}

	for (int i = 0; i < bad_jobs; i++) {
		manager.enqueue<int>(exception_job_function, job_callback);
	}

	while (finished.load() < good_jobs + bad_jobs) {
		manager.execute_callbacks();
	}

	manager.stop();

	if (errors.load() == bad_jobs) {
		return 0;
	}
	return -1;
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
	}
	return;
out:
	log::log(MSG(err) << testname << " failed with return value " << ret);
	throw "job manager tests failed";
}


}}} // namespace openage::job::tests
