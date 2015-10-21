// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/** @file
 * render task implementation to tell the renderer to
 * perform any drawing on the screen.
 */

#include "task.h"


namespace openage {
namespace renderer {


bool Task::operator <(const Task &other) const {
	return true;
}

TaskState::TaskState(Task *task, Renderer *renderer)
	:
	task{task},
	renderer{renderer} {}

}} // openage::renderer
