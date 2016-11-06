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
	virtual void prepare() override;

	/**
	 * Returns the SDL window flags for the opengl window.
	 */
	virtual uint32_t get_window_flags() const override;

	/**
	 * Actually creates the OpenGL context for the given SDL window.
	 */
	virtual void create(SDL_Window *window) override;

	/**
	 * Setup calls for the newly created context.
	 *
	 * Enables opengl functions like blending etc.
	 */
	virtual void setup() override;

	/**
	 * Deinitializes and unregisters the gl context from SDL2.
	 */
	virtual void destroy() override;

	/**
	 * use glEnable and glDisable to toggle a given feature.
	 */
	virtual void set_feature(context_feature feature, bool on) override;

	/**
	 * Creates the opengl texture in this context.
	 * @returns a handle to it.
	 */
	virtual std::shared_ptr<renderer::Texture> register_texture(const TextureData &data) override;

	/**
	 * Register a glsl shader pipeline program to the context.
	 * @returns a handle to the new program.
	 */
	virtual std::shared_ptr<renderer::Program> register_program(const ProgramSource &data) override;


protected:
	/**
	 * SDL opengl context state.
	 */
	SDL_GLContext glcontext;
};

}}} // namespace openage::renderer

#endif
