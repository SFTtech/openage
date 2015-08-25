// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_PROGRAM_H_
#define OPENAGE_RENDERER_PROGRAM_H_

#include <memory>
#include <unordered_map>
#include <vector>

namespace openage {
namespace renderer {

class Context;
class ShaderSource;
class Texture;


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

	/**
	 * Use this program now on the GPU.
	 */
	virtual void use() = 0;

	/**
	 * Dump vertex attribute variables.
	 */
	virtual void dump_attributes() = 0;

	/* ========================================== */
	// available pipeline properties

	/**
	 * Set a 3 dimensional float vector
	 */
	virtual void set_uniform_3f(const char *name, const std::array<float, 3> &value) = 0;

	/**
	 * Set 2d texture data.
	 */
	virtual void set_uniform_2dtexture(const char *name, const Texture &value) = 0;

	/* ========================================== */

protected:
	/**
	 * The associated context.
	 */
	Context *context;
};

}} // openage::renderer

#endif
