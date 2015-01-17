// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_STATE_BASE_H_
#define OPENAGE_JOB_JOB_STATE_BASE_H_

namespace openage {
namespace job {

/**
 * An abstract base class for a shared state of a Job. The real shared state
 * implementation is done in JobState<T>. This is necessary in order to be able
 * to store generic JobStates within the same container in the JobManager.
 */
class JobStateBase {
public:
	/** Default constructor. */
	virtual ~JobStateBase() = default;

	/** This function executes the Job. */
	virtual void execute() = 0;

	/** TODO */
	virtual void execute_callback() = 0;

	virtual unsigned get_thread_id() = 0;
};

}
}

#endif
