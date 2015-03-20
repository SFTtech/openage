// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_ABORTED_EXCEPTION_H_
#define OPENAGE_JOB_JOB_ABORTED_EXCEPTION_H_

#include <stdexcept>

namespace openage {
namespace job {

/** An exception that is thrown when a job wants to abort itself. */
class JobAbortedException : public std::exception {
public:
	virtual const char *what() const noexcept {
		return "job aborted";
	}
};

}} // openage::job

#endif
