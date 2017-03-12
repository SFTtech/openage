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

class Uniform {
public:
	
	
private:

	void* value;
};

class UniformMap {
public:
	static UniformMap defaults_from_shader(Shader const&);


	void add_uniform(Uniform, value);

	void remove_uniform(Uniform);

	void set_uniform();

private:
	map<string, uniform> uniforms;

	shader; <- uniforms

	uniform values; <- pos, rot
};

/**
 * Instructs the renderer to draw something.
 */
class Task {
public:
	void set_geometry(Geometry const*);

	/**
	 * The concept of materials is as follows:
	 * - a material is defined as a mapping between shader uniforms and their values
	 * - more than one material can be applied to a model, provided there are no two materials map the same uniform to a value
	 * Materials can be, for example, used like this:
	 * one shared texture and color material between many similar-looking objects
	 * each object has its own transform matrix material
	 */
	void add_material(Material const*);


	/**
	 * This immediately invalidates all attached materials and removes them.	*/
	void set_shader(Shader const*);

	/**
	 * This operation preserves all materials that fit the new shader.
	 */
	void set_shader_preserve_mappings(Shader const*);
	
	
private:
	bool geometry_dirty;
	/**
	 * The geometry to draw.
	 */
	Geometry const* geometry;

	/**
	 * The material to use for drawing the geometry.
	 * Stores to-be-set uniforms.
	 * Can create vertex buffers.
	 */
	//MaterialApplication material;

	bool position_dirty;
	/**
	 * Position to draw the geometry at.
	 */
	coord::phys3 position;

	bool rotation_dirty;
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
