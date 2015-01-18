// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_JOB_ABORTED_EXCEPTION_H_
#define OPENAGE_JOB_JOB_ABORTED_EXCEPTION_H_

#include <stdexcept>

namespace openage {
namespace job {

class JobAbortedException : public std::exception {
public:
	JobAbortedException() = default;
	~JobAbortedException() = default;
};

}
}

#endif
