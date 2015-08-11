// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_SHADER_H_
#define OPENAGE_RENDERER_SHADER_H_

/** @file
 * gpu program creation classes reside in here.
 */

#include <string>
#include <vector>

namespace openage {
namespace renderer {

/**
 * Available shader types present in modern graphics pipelines.
 */
enum class shader_type {
	fragment,
	vertex,
	geometry,
	tesselation_control,
	tesselation_evaluation,
};


/**
 * Shader source code storage.
 */
class ShaderSource {
public:
	ShaderSource(shader_type type);
	virtual ~ShaderSource() {};

	/**
	 * Return the shader source code.
	 */
	const char *get_source() const;

	/**
	 * Return the shader type.
	 */
	shader_type get_type() const;

protected:
	/**
	 * The shader's pipeline stage position.
	 */
	shader_type type;

	/**
	 * Stores the shader source code.
	 */
	std::string code;
};


/**
 * Shader source code obtained from a file.
 */
class ShaderSourceFile : public ShaderSource {
public:
	ShaderSourceFile(shader_type type, const std::string &path);
	virtual ~ShaderSourceFile() {};

protected:
	/**
	 * The shader's filesystem location.
	 * TODO: replace by some path magic
	 */
	std::string path;
};


/**
 * Shader source code obtained directly as text.
 */
class ShaderSourceCode : public ShaderSource {
public:
	ShaderSourceCode(shader_type type, const char *code);
	ShaderSourceCode(shader_type type, const std::string &code);

	virtual ~ShaderSourceCode() {};
};


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
	Program();

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
};

}} // openage::renderer

#endif
