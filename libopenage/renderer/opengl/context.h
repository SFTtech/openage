// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <SDL2/SDL.h>


namespace openage {
namespace renderer {
namespace opengl {

/// Stores information about context capabilities and limitations.
struct gl_context_capabilities {
	/// The maximum number of vertex attributes in a shader.
	size_t max_vertex_attributes;
	/// The amount of texture units (GL_TEXTUREi) available.
	size_t max_texture_slots;
	/// The maximum size of a single dimension of a texture.
	size_t max_texture_size;

	int major_version;
	int minor_version;
};

/// Manages an OpenGL context.
class GlContext {
public:
	/// Create a GL context in the given SDL window.
	explicit GlContext(SDL_Window*);
	~GlContext();

	/// It doesn't make sense to have more than one instance of the same context.
	GlContext(const GlContext&) = delete;
	GlContext& operator=(const GlContext&) = delete;

	/// We have to support moving to avoid a mess somewhere else.
	GlContext(GlContext&&);
	GlContext& operator=(GlContext&&);

	/// Returns the underlying SDL context pointer.
	SDL_GLContext get_raw_context() const;

	/// Returns the capabilities of this context.
	gl_context_capabilities get_capabilities() const;

	/// Turns VSYNC on or off for this context.
	void set_vsync(bool on);

	/// Checks whether the current GL context on this thread reported any errors
	/// and throws an exception if it did. Note that it's static.
	static void check_error();

private:
	/// Pointer to SDL struct representing the GL context.
	SDL_GLContext gl_context;

	/// Context capabilities.
	gl_context_capabilities capabilities;
};

}}} // openage::renderer::opengl
