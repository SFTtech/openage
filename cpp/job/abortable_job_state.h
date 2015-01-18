// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_ABORTABLE_JOB_STATE_H_
#define OPENAGE_JOB_ABORTABLE_JOB_STATE_H_

#include <functional>

#include "job_aborted_exception.h"
#include "typed_job_state_base.h"
#include "types.h"

namespace openage {
namespace job {

template<class T>
class AbortableJobState : public TypedJobStateBase<T> {
public:
	abortable_function_t<T> function;

	AbortableJobState(abortable_function_t<T> function,
			callback_function_t<T> callback)
			:
			TypedJobStateBase<T>{callback},
			function{function} {
	}

	virtual ~AbortableJobState() = default;

protected:
	virtual T execute_and_get(should_abort_t should_abort) {
		static auto abort = []() {
			throw JobAbortedException{};
		};
		return std::move(this->function(should_abort, abort));
	}
};

}
}

#endif
