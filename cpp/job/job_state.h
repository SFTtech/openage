#ifndef OPENAGE_JOB_JOB_STATE_H_
#define OPENAGE_JOB_JOB_STATE_H_

#include <atomic>
#include <exception>
#include <functional>

namespace openage {
namespace job {

// abstract base class 
class BaseJobState {
public:
	virtual ~BaseJobState() = default;

	virtual void execute() = 0;
};

template<class T>
class JobState : public BaseJobState {
public:
	// the jobs function
	std::function<T()> function;
	// whether the job has finished
	std::atomic_bool finished;

	// the job's result
	T result;
	// if an exception occured it will be stored here
	std::exception_ptr exception;

	JobState(std::function<T()> function)
			:
			function{function},
			finished{false} {
	}

	virtual ~JobState() = default;

	/**
	 * Executes this job state's internal function and stores its result. If an
	 * exception is thrown, the corresponding exception_ptr will be stored.
	 */
	virtual void execute() {
		try {
			this->result = this->function();
		} catch (...) {
			this->exception = std::current_exception();
		}
		this->finished.store(true);
	}
};

}
}

#endif
