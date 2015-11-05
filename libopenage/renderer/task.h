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
	/**
	 * The geometry to draw.
	 */
	Geometry *const geometry;

	/**
	 * The material to use for drawing the geometry.
	 * Stores to-be-set uniforms.
	 * Can create vertex buffers.
	 */
	//MaterialApplication material;

	/**
	 * Position to draw the geometry at.
	 */
	coord::phys3 position;

	/**
	 * Rotation for the geometry.
	 */
	util::Quaternion<> rotation;

	/**
	 * Ordering condition to draw tasks in the correct order.
	 */
	bool operator <(const Task &other) const;
};


/**
 * corresponding state storage for a render task.
 */
class TaskState {
public:
	TaskState(Task *t, Renderer *r);
	~TaskState();

	/**
	 * true if the assigned task has been rendered on screen.
	 */
	bool rendered;

protected:
	Task *const task;
	Renderer *const renderer;
};


}} // openage::renderer

#endif
