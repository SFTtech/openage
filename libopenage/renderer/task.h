// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TASK_H_
#define OPENAGE_RENDERER_TASK_H_

#include "../coord/phys3.h"
#include "../util/quaternion.h"

namespace openage {
namespace renderer {

class Geometry;
class Material;
class Renderer;


/**
 * Instructs the renderer to draw something.
 */
class Task {
public:
	Material *material;
	Geometry *geometry;

	coord::phys3 position;
	util::Quaternion<> rotation;

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
	Renderer *renderer;
};

/**
 * Groups some tasks together.
 * The order of tasks in this group is optimized.
 *
 * E.g. all unit draw actions are in one group.
 */
class TaskGroup {
public:
	TaskGroup();
	~TaskGroup();

protected:
	std::vector<Task> tasks; // heap?
	size_t id;
};


}} // namespace openage::renderer

#endif
