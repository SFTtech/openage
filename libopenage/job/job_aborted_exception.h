// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <stdexcept>

namespace openage {
namespace job {

/** An exception that is thrown when a job wants to abort itself. */
class JobAbortedException : public std::exception {
public:
	const char *what() const noexcept override {
		return "job aborted";
	}
};

}} // openage::job
