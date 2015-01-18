// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_TYPED_JOB_STATE_BASE_H_
#define OPENAGE_JOB_TYPED_JOB_STATE_BASE_H_

#include <atomic>
#include <exception>
#include <functional>

#include "job_aborted_exception.h"
#include "job_state_base.h"
#include "thread_id.h"
#include "types.h"

namespace openage {
namespace job {

template<class T>
class TypedJobStateBase : public JobStateBase {
public:
	/** Id of the thread, that created this job state. */
	unsigned thread_id;
	
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

	TypedJobStateBase(callback_function_t<T> callback)
			:
			thread_id{openage::job::thread_id.id},
			callback{callback},
			finished{false} {
	}

	virtual ~TypedJobStateBase() = default;

	/**
	 * Executes the internal function object and stores its result. Occuring
	 * exceptions are stored, as well. Returns whether this job has been
	 * aborted.
	 */
	virtual bool execute(should_abort_t should_abort) {
		try {
			this->result = std::move(this->execute_and_get(should_abort));
		} catch (JobAbortedException &e) {
			return true;
		} catch (...) {
			this->exception = std::current_exception();
		}
		this->finished.store(true);
		return false;
	}

	virtual void execute_callback() {
		// TODO assure finished
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

	virtual unsigned get_thread_id() {
		return this->thread_id;
	}

protected:
	virtual T execute_and_get(should_abort_t should_abort) = 0;
};

}
}

#endif
