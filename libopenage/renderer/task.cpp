// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * common code for all render tasks.
 */

#include "task.h"


namespace openage {
namespace renderer {


bool Task::operator <(const Task &other) const {
	if (this->position < other.position) {
		return true;
	} else {
		return false;
	}
}

TaskState::TaskState() {}

}} // openage::renderer
