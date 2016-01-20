// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include "job_aborted_exception.h"
#include "typed_job_state_base.h"
#include "types.h"

namespace openage {
namespace job {


/**
 * An abortable job state supports job that can be aborted. This is done by
 * providing two function objects to the job's function. One is used to check
 * whether the job should be aborted, while the other one aborts the job.
 */
template<class T>
class AbortableJobState : public TypedJobStateBase<T> {
public:
	/** The job's function. */
	abortable_function_t<T> function;

	/** Creates a new abortable job with the given function and callback. */
	AbortableJobState(abortable_function_t<T> function,
                      callback_function_t<T> callback)
		:
		TypedJobStateBase<T>{callback},
		function{function} {
	}

	/** Default destructor. */
	virtual ~AbortableJobState() = default;

protected:
	virtual T execute_and_get(should_abort_t should_abort) {
		// abort is a function, that can be used to abort a running job. It is
		// passed to the executing function.
		static auto abort = []() {
			throw JobAbortedException{};
		};
		// should_abort is a function, that returns, whether a running job
		// should be aborted.
		return std::move(this->function(should_abort, abort));
	}
};


}} // namespace openage::job
