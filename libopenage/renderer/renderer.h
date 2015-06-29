// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_RENDERER_H_
#define OPENAGE_RENDERER_RENDERER_H_

#include "context.h"
#include "task.h"

namespace openage {
namespace renderer {

/**
 * Main for collecting and rendering renderable things.
 *
 * All tasks are added and aggregated,
 * when the render is invoked, the tasks are sorted and executed.
 */
class Renderer {
public:
	Renderer(const Renderer &other) = delete;
	Renderer(Renderer &&other) = delete;
	Renderer &operator =(const Renderer &other) = delete;
	Renderer &operator =(Renderer &&other) = delete;

	Renderer();
	~Renderer();

	void add_task(task task);
	void render();

	void register_shader(const Shader &shader);

private:
	std::vector<task> instructions_sorted();

	/**
	 * All tasks the renderer has to to display on the next drawout
	 */
	std::vector<task> tasks;
};

}} // namespace openage::renderer

#endif
