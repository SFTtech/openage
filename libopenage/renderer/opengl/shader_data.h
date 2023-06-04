// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>

#include <epoxy/gl.h>

namespace openage::renderer::opengl {
/**
 * Represents a uniform in the default (shader) block, i.e. not within a named block.
 */
struct GlUniform {
	GLenum type;

	/**
     * Location of the uniform for use with glUniform and glGetUniform.
	 * NOT the same as the uniform index.
     */
	GLuint location;
};

/**
 * Represents a uniform in a named block.
 */
struct GlInBlockUniform {
	GLenum type;

	/**
     * Offset relative to the beginning of the block at which this uniform is placed.
     */
	size_t offset;

	/**
     * The size in bytes of the whole uniform. If the uniform is an array,
	 * the size of the whole array.
     */
	size_t size;

	/**
     * Only relevant for arrays and matrices.
	 * In arrays, specifies the distance between the start of each element.
	 * In row-major matrices, specifies the distance between the start of each row.
	 * In column-major matrices, specifies the distance between the start of each column.
     */
	size_t stride;

	/**
     * Only relevant for arrays. The number of elements in the array.
     */
	size_t count;
};

/**
 * Represents a uniform block in a shader program.
 */
struct GlUniformBlock {
	GLuint index;

	/**
     * Size of the entire block. How uniforms are packed within depends
	 * on the block layout and is described in corresponding GlUniforms.
     */
	size_t data_size;

	/**
     * Maps uniform names within this block to their descriptions.
     */
	std::unordered_map<std::string, GlInBlockUniform> uniforms;

	/**
     * The binding point assigned to this block.
     */
	GLuint binding_point;
};

/**
 * Represents a per-vertex input to the shader program.
 */
struct GlVertexAttrib {
	GLenum type;
	GLint location;
	// TODO what is this?
	GLint size;
};

} // namespace openage::renderer::opengl
