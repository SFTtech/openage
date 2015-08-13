// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_PROGRAM_H_
#define OPENAGE_RENDERER_PROGRAM_H_

#include <memory>
#include <vector>

namespace openage {
namespace renderer {

class ShaderSource;

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
class RawProgram {
protected:
	RawProgram();

public:
	virtual ~RawProgram() {};

	/**
	 * Use this program now on the GPU.
	 */
	virtual void use() = 0;

	/**
	 * Dump vertex attribute variables.
	 */
	virtual void dump_attributes() = 0;
};


/**
 * Inherit from this class to create statically known program properties.
 *
 * Add members of ProgramVariable to the inherited class
 * to make pipeline variables available to the outside.
 */
class Program {
public:
	Program(std::unique_ptr<RawProgram> prg);
	virtual ~Program();

protected:
	/**
	 * Add the given program variable to the list of maintained
	 * pipeline attributes.
	 */
	void add_var(const ProgramVariable &var);

	/**
	 * The pipeline program associated with this property definition class.
	 */
	std::unique_ptr<RawProgram> raw_program;
};


}} // openage::renderer

#endif
