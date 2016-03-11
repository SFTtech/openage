// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <atomic>
#include <exception>
#include <functional>

#include "../util/thread_id.h"
#include "../error/error.h"
#include "job_aborted_exception.h"
#include "job_state_base.h"
#include "types.h"

namespace openage {
namespace job {

/**
 * A templated job state base class, that implements basic functionality of job
 * states.
 *
 * @param T the result type of this job state. This type must have a default
 *		constructor and support move semantics.
 */
template<class T>
class TypedJobStateBase : public JobStateBase {
public:
	/** Id of the thread, that created this job state. */
	size_t thread_id;

	/**
	 * A callback function that is called when the job has finished. Can be
	 * empty.
	 */
	callback_function_t<T> callback;

	/**
	 * Whether the Job's execution has already been finished. An atomic_bool is
	 * used, as this field can be used by multiple threads. Thus explicit
	 * synchronization is avoided.
	 */
	std::atomic_bool finished;

	/** The result of the Job's executed function. */
	T result;

	/** If executing the Job throws an exception, it is stored here. */
	std::exception_ptr exception;

	/** Creates a new typed job with the given callback. */
	TypedJobStateBase(callback_function_t<T> callback)
		:
		thread_id{openage::util::get_current_thread_id()},
		callback{callback},
		finished{false} {
	}

	/** Default destructor. */
	virtual ~TypedJobStateBase() = default;

	/**
	 * Executes the internal function object and stores its result. Occuring
	 * exceptions are stored, as well. Returns whether this job has been
	 * aborted.
	 */
	bool execute(should_abort_t should_abort) override {
		try {
			this->result = this->execute_and_get(should_abort);
		} catch (JobAbortedException &e) {
			return true;
		} catch (...) {
			this->exception = std::current_exception();
		}
		this->finished.store(true);
		return false;
	}

	void execute_callback() override {
		ENSURE(this->finished.load(), "trying to report a result of an unfinished job");
		if (this->callback) {
			auto get_result = [this]() {
				if (this->exception != nullptr) {
					std::rethrow_exception(this->exception);
				} else {
					return std::move(this->result);
				}
			};
			this->callback(get_result);
		}
	}

	size_t get_thread_id() override {
		return this->thread_id;
	}

protected:
	/**
	 * Executes the job and returns the result. If an exception is thrown it
	 * must be passed to the calling function.
	 */
	virtual T execute_and_get(should_abort_t should_abort) = 0;
};

}
}
