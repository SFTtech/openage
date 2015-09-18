// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_PROGRAM_H_
#define OPENAGE_RENDERER_PROGRAM_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "../util/vector.h"

namespace openage {
namespace renderer {

class Context;
class ShaderSource;
class Texture;
class VertexBuffer;


/**
 * Create this to assemble shaders to a usable program.
 * Register it at the renderer to use it.
 *
 * A program is a graphics card kernel, consisting of multiple
 * shaders used for the rendering stages.
 */
class ProgramSource {
public:
	ProgramSource();
	ProgramSource(const std::vector<const ShaderSource *> &shaders);
	virtual ~ProgramSource() {};

	/**
	 * Attach the given shader to this program.
	 */
	void attach_shader(const ShaderSource &shader);

	/**
	 * Return the list of all assigned shaders to this program.
	 */
	const std::vector<const ShaderSource *> &get_shaders() const;

protected:
	/**
	 * All attached shaders to this program.
	 */
	std::vector<const ShaderSource *> shaders;
};


/**
 * A usable handle, aquired by registering the sources of the program
 * to the renderer and getting back an object of this class.
 */
class Program {
protected:
	Program(Context *context);

public:
	virtual ~Program() {};

	Context *get_context();

	/**
	 * Use this program now on the GPU.
	 */
	virtual void use() = 0;

	/**
	 * Dump vertex attribute variables.
	 */
	virtual void dump_attributes() = 0;

	/**
	 * Return the glsl layout id for a given vertex attribute name.
	 */
	virtual int get_attribute_id(const char *name) = 0;

	/* ========================================== */
	// available pipeline properties

	/**
	 * Upload a vertex buffer to the gpu
	 */
	virtual void set_vertex_buffer(const VertexBuffer &buf) = 0;

	/**
	 * Set a 3 dimensional float vector
	 */
	virtual void set_uniform_3f(const char *name, const util::Vector<3> &value) = 0;

	/**
	 * Set a single integer value
	 */
	virtual void set_uniform_1i(const char *name, const int &value) = 0;

	/**
	 * Set 2d texture data.
	 */
	virtual void set_uniform_2dtexture(const char *name, const Texture &value) = 0;

	/* ========================================== */

	/**
	 * The associated context.
	 */
	Context *const context;
};

}} // openage::renderer

#endif
