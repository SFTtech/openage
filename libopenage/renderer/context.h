// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_CONTEXT_H_
#define OPENAGE_RENDERER_CONTEXT_H_

#include <SDL2/SDL.h>

#include <memory>

#include "shader.h"
#include "texture.h"

namespace openage {
namespace renderer {

/**
 * Available context types.
 * Specifies all available backends.
 * autodetect: use vulkan > opengl. could use some tuning :D
 */
enum class context_type {
	autodetect,
	vulkan,
	opengl,
};

/**
 * Context features that can be enabled or disabled.
 * Used in Context::set_feature
 */
enum class context_feature {
	blending,        //!< Alpha blending
	depth_test,      //!< z-buffering for fragment depth tests
};

class Context {
public:
	Context(context_type type);
	virtual ~Context() = default;

	/**
	 * Create a context according to the requested type.
	 */
	static std::unique_ptr<Context> generate(context_type t);

	/**
	 * Called before the drawing window is created.
	 */
	virtual void prepare() = 0;

	/**
	 * Returns the SDL2 window flags for the requested context type.
	 */
	virtual uint32_t get_window_flags() const = 0;

	/**
	 * Actually creates the requested context for the given SDL window.
	 */
	virtual void create(SDL_Window *window) = 0;

	/**
	 * Setup calls for the newly created context.
	 * After creation, the context may want to have some setup
	 * calls that configure functionality.
	 */
	virtual void setup() = 0;

	/**
	 * Destroy the context to unregister it.
	 */
	virtual void destroy() = 0;

	/**
	 * Enable or disable a given context feature.
	 * @param feature the feature to modify
	 * @param on whether to set this feature on or off.
	 */
	virtual void set_feature(context_feature feature, bool on) = 0;

	/**
	 * Register some texture data to the context.
	 * @returns the newly created Texture handle.
	 */
	virtual std::shared_ptr<Texture> register_texture(const TextureData &data) = 0;

	/**
	 * Register some shader pipeline program to the context.
	 * @returns the newly created Program handle.
	 */
	virtual std::shared_ptr<Program> register_program(const ProgramSource &data) = 0;


protected:
	/**
	 * Type of this context, namely the backend variant.
	 */
	context_type type;
};

}} // namespace openage::renderer

#endif
