// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_RENDERER_H_
#define OPENAGE_RENDERER_RENDERER_H_

#include "../datastructure/pairing_heap.h"
#include "../handlers.h"
#include "task.h"

namespace openage {

/**
 * Contains all components for the graphics renderer.
 * Supports multiple backends and provides a uniform interface for
 * interacting with the GPU and the render window.
 *
 * It gathers tasks to be rendered in a heap.
 * This orders the instructions in an optimized order,
 * so that the GPU performs as few context switches as possible.
 */
namespace renderer {

class Context;
class ProgramSource;
class Program;
class Texture;
class TextureData;


/**
 * Main for collecting and rendering renderable things.
 *
 * All tasks are added and aggregated,
 * when the render is invoked, the tasks are sorted and executed.
 */
class Renderer : public ResizeHandler {
public:
	/**
	 * A renderer has to be created for a context.
	 */
	Renderer(const std::shared_ptr<Context> &ctx);
	virtual ~Renderer();

	Renderer(const Renderer &other) = delete;
	Renderer(Renderer &&other) = delete;
	Renderer &operator =(const Renderer &other) = delete;
	Renderer &operator =(Renderer &&other) = delete;

	/**
	 * Add a task to be rendered to the job list.
	 * This enqueues the task to be drawn in an optimized order.
	 *
	 * @returns the state handle for the added task.
	 */
	TaskState add_task(Task &task);

	/**
	 * Register a pipeline program to this renderer.
	 *
	 * Internally, this creates the internal backend-aware instance for
	 * the program.
	 *
	 * @returns a handle to the usable program for the developer.
	 */
	std::unique_ptr<Program> add_program(const ProgramSource &source);

	/**
	 * Register a texture to the renderer.
	 * Creates the context-aware handling structures.
	 *
	 * @returns a texture handle to be used in the code.
	 */
	std::unique_ptr<Texture> add_texture(const TextureData &data);

	/**
	 * Take a screenshot of the current framebuffer.
	 * Save it as png to the given filename.
	 */
	void screenshot(const std::string &filename);

	/**
	 * Test if the renderer has an invalid state.
	 */
	void check_error();

	/**
	 * Resize the renderer because the surrounding window size was updated.
	 */
	bool on_resize(coord::window new_size) override;

	/**
	 * Finally, this is the actual drawing invocation.
	 * Pushes all the aggregated data to the GPU and renders it.
	 * When this is done, the front and backbuffer are flipped.
	 */
	void render() const;

protected:
	/**
	 * All tasks the renderer has to to display on the next drawout
	 */
	datastructure::PairingHeap<Task> tasks;

	/**
	 * The context to which the renderer is attached to.
	 */
	std::shared_ptr<Context> context;
};

}} // namespace openage::renderer

#endif
