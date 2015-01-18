// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_ABORTABLE_JOB_STATE_H_
#define OPENAGE_JOB_ABORTABLE_JOB_STATE_H_

#include <functional>

#include "typed_job_state_base.h"

namespace openage {
namespace job {

template<class T>
class AbortableJobState : public TypedJobStateBase<T> {
public:
	std::function<T(std::function<bool()>)> function;

	AbortableJobState(std::function<T(std::function<bool()>)> function,
			std::function<void(T)> callback)
			:
			TypedJobStateBase<T>{callback},
			function{function} {
	}

	virtual ~AbortableJobState() = default;

protected:
	virtual T execute_and_get(std::function<bool()> abort) {
		return std::move(this->function(abort));
	}
};

}
}

#endif
