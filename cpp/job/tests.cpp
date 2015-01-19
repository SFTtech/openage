// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "job_manager.h"
#include "../log.h"

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

	while(!finished) {
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

	while(!finished) {
		manager.execute_callbacks();
	}

	manager.stop();
	return result;

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
	log::err("%s failed with return value %d", testname, ret);
	throw "job manager tests failed";
}

}
}
}
