// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TASK_H_
#define OPENAGE_RENDERER_TASK_H_

#include "material.h"

namespace openage {
namespace renderer {

/**
 * render layer, their order is important.
 * layers from bottom to top: later in enum = drawn later
 */
enum class layer {
	terrain,
	unit,
	sky,
	hud,
};

/**
 * struct to submit to the renderer
 */
class Task {
public:
	layer position;
	std::vector<Material> materials;

	bool operator <(const Task &other) const;
};


/**
 * corresponding state storage for a render task.
 */
class TaskState {
public:
	TaskState();
	~TaskState();

	/**
	 * true if the assigned task has been rendered on screen.
	 */
	bool rendered;

private:
	Task *task;
	class Renderer *renderer;
};


}} // namespace openage::renderer

#endif
