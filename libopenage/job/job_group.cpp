// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "job_group.h"

namespace openage::job {

JobGroup::JobGroup()
	:
	JobGroup{nullptr} {
}

JobGroup::JobGroup(Worker *parent_worker)
	:
	parent_worker{parent_worker} {
}

} // namespace openage::job
