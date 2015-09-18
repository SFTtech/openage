// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_VERTEX_BUFFER_H_
#define OPENAGE_RENDERER_VERTEX_BUFFER_H_

#include "buffer.h"

#include <vector>

namespace openage {
namespace renderer {

class Context;

/**
 * Represents a graphics vertex buffer.
 * This can be uploaded to the GPU to provide vertex-specific variables
 * in a shader.
 */
class VertexBuffer {
public:
	/**
	 * Describes a data section in a vertex buffer.
	 */
	struct vbo_section {
		int attr_id;          //!< vertex attribute layout id.
		size_t entry_width;   //!< number of chars for one vertex attribute
		size_t offset;        //!< start offset in the buffer
	};

	VertexBuffer(Context *ctx);
	virtual ~VertexBuffer() = default;

	/**
	 * Upload this buffer to the GPU and bind it to the
	 * required vertex attribute slot.
	 */
	void upload() const;

	/**
	 * Allocate a buffer of given size.
	 */
	void alloc(size_t size);

	/**
	 * Reset the metadata, leave data intact.
	 * To destroy the data buffer, call alloc again.
	 */
	void reset();

	/**
	 * Add a single vertex buffer metadata entry.
	 */
	void add_section(const vbo_section &section);

	/**
	 * Return the raw pointer to the allocated buffer.
	 */
	char *get();

	/**
	 * Return the underlying context-specific buffer object.
	 */
	Buffer *get_buffer();

	/**
	 * List of vertex buffer sections describing the data width and types.
	 */
	std::vector<vbo_section> sections;

protected:
	std::unique_ptr<Buffer> buffer;
};

}} // openage::renderer

#endif
