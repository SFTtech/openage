// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "job_group.h"

namespace openage {
namespace job {

JobGroup::JobGroup()
		:
		JobGroup{nullptr} {
}

JobGroup::JobGroup(Worker *parent_worker)
		:
		parent_worker{parent_worker} {
}

}
}
