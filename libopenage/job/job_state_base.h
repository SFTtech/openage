// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include "types.h"

namespace openage {
namespace job {

/**
 * An abstract base class for a shared state of a job. A job state keeps track
 * of its execution state and store's the job's result. Further it keeps track
 * of exceptions that occurred during the job's execution. The real shared state
 * implementation is done in templated subclasses. This is necessary to support
 * arbitrary result types.
 */
class JobStateBase {
public:
	/** Default constructor. */
	virtual ~JobStateBase() = default;

	/**
	 * This function executes the job. It returns whether the job has been
	 * aborted.
	 */
	virtual bool execute(should_abort_t should_abort) = 0;

	/**
	 * Executes the job's callback, if a callback function has been provided
	 * while constructing this job. This function may only be called if the job
	 * has already finished.
	 */
	virtual void execute_callback() = 0;

	/** Returns the id of the thread that has created this job. */
	virtual size_t get_thread_id() = 0;
};

}
}
