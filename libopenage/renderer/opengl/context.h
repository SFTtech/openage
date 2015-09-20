// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_CONTEXT_H_
#define OPENAGE_RENDERER_OPENGL_CONTEXT_H_

#include <SDL2/SDL.h>
#include <memory>

#include "../context.h"


namespace openage {
namespace renderer {

/**
 * OpenGL specific renderer code.
 * Is selected if the requested backend is OpenGL.
 */
namespace opengl {

class Context : public renderer::Context {
public:
	Context();
	~Context();

	/**
	 * Called before the drawing window is created.
	 */
	void prepare() override;

	/**
	 * Returns the SDL window flags for the opengl window.
	 */
	uint32_t get_window_flags() const override;

	/**
	 * Actually creates the OpenGL context for the given SDL window.
	 */
	void create(SDL_Window *window) override;

	/**
	 * Setup calls for the newly created context.
	 *
	 * Enables opengl functions like blending etc.
	 */
	void setup() override;

	/**
	 * Deinitializes and unregisters the gl context from SDL2.
	 */
	void destroy() override;

	/**
	 * use glEnable and glDisable to toggle a given feature.
	 */
	void set_feature(context_feature feature, bool on) override;

	/**
	 * Read the opengl framebuffer and dump it to a png file.
	 */
	void screenshot(const std::string &filename) override;

	/**
	 * Creates the opengl texture in this context.
	 * @returns a handle to it.
	 */
	std::unique_ptr<renderer::Texture> register_texture(const TextureData &data) override;

	/**
	 * Register a glsl shader pipeline program to the context.
	 * @returns a handle to the new program.
	 */
	std::unique_ptr<renderer::Program> register_program(const ProgramSource &data) override;

	/**
	 * Create a opengl buffer handle
	 * @returns the newly created state object.
	 */
	std::unique_ptr<renderer::Buffer> create_buffer(size_t size=0) override;

	/**
	 * Create an opengl vertex state tracker
	 */
	std::unique_ptr<renderer::VertexState> create_vertex_state() override;


	/**
	 * Return the available OpenGL context properties and limitations.
	 */
	context_capability get_capabilities() override;

protected:
	/**
	 * Resize the opengl viewport.
	 */
	void resize_canvas(const coord::window &new_size) override;

	/**
	 * SDL opengl context state.
	 */
	SDL_GLContext glcontext;
};

}}} // namespace openage::renderer

#endif
