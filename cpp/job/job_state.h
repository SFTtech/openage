// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_STATE_H_
#define OPENAGE_JOB_JOB_STATE_H_

#include <atomic>
#include <exception>
#include <functional>

namespace openage {
namespace job {

/**
 * An abstract base class for a shared state of a Job. The real shared state
 * implementation is done in JobState<T>. This is necessary in order to be able
 * to store generic JobStates within the same container in the JobManager.
 */
class BaseJobState {
public:
	/** Default constructor. */
	virtual ~BaseJobState() = default;

	/** This function executes the Job. */
	virtual void execute() = 0;
};

/**
 * A JobState is the internal state of a Job. It keeps track of its execution
 * state and stores the Job's result. Further it keeps track of exceptions that
 * occured during the Job's execution.
 * A JobState is created by a JobManager and passed to the user by a lighweight
 * proxy Job object.
 *
 * @param T the result type of this JobState. This type must have a default
 *		constructor and support move semantics.
 */
template<class T>
class JobState : public BaseJobState {
public:
	/** A function object which is executed by the JobManager. */
	std::function<T()> function;

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

	/**
	 * Creates a new JobState with the given function, that is to be executed.
	 */
	JobState(std::function<T()> function)
			:
			function{function},
			finished{false} {
	}

	/** Default destructor. */
	virtual ~JobState() = default;

	/**
	 * Executes the internal function object and stores its result. Occuring
	 * exceptions are stored, as well.
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
