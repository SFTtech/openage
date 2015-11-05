// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_VERTEX_BUFFER_H_
#define OPENAGE_RENDERER_VERTEX_BUFFER_H_

#include <vector>

#include "buffer.h"
#include "../datastructure/constexpr_map.h"

namespace openage {
namespace renderer {

class Context;


/**
 * The data type of a vertex attribute.
 */
enum class vertex_attribute_type {
	integer_8,
	integer_16,
	integer_32,
	uinteger_8,
	uinteger_16,
	uinteger_32,
	float_32,
	fixed_16_16,
};

constexpr auto vertex_attribute_size = datastructure::create_const_map<vertex_attribute_type, size_t>(
	std::make_pair(vertex_attribute_type::integer_8, 1),
	std::make_pair(vertex_attribute_type::integer_16, 2),
	std::make_pair(vertex_attribute_type::integer_32, 4),
	std::make_pair(vertex_attribute_type::uinteger_8, 1),
	std::make_pair(vertex_attribute_type::uinteger_16, 2),
	std::make_pair(vertex_attribute_type::uinteger_32, 4),
	std::make_pair(vertex_attribute_type::float_32, 4),
	std::make_pair(vertex_attribute_type::fixed_16_16, 4)
);

/**
 * Represents a graphics vertex buffer.
 * A a vertex buffer supplies potentially different information
 * for each vertex, e.g. its position, texture coordinates, color, etc.
 *
 * This is uploaded to the GPU to provide vertex-specific variables
 * in a shader.
 */
class VertexBuffer {
public:

	/**
	 * Describes a data section in a vertex buffer.
	 */
	struct vbo_section {
		int attr_id;                 //!< attribute layout id.
		vertex_attribute_type type;  //!< attribute data type
		size_t dimension;            //!< attribute dimension
		size_t offset;               //!< start offset in the buffer
	};

	VertexBuffer(Context *ctx,
	             Buffer::usage usage=Buffer::usage::static_draw);

	VertexBuffer(VertexBuffer &&other);
	const VertexBuffer &operator =(VertexBuffer &&other);

	virtual ~VertexBuffer() = default;

	/**
	 * Upload this buffer to the GPU and bind it to the
	 * required vertex attribute buffer slot.
	 */
	void upload();

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
	char *get(bool will_modify=false);

	/**
	 * Return the buffer metadata describing layout and position
	 * of buffer areas.
	 */
	const std::vector<vbo_section> &get_sections() const;

	/**
	 * Return the underlying context-specific buffer object.
	 */
	Buffer *get_buffer();

protected:
	/**
	 * List of vertex buffer sections describing the data width and types.
	 */
	std::vector<vbo_section> sections;

	/**
	 * Associated raw gpu buffer.
	 */
	std::unique_ptr<Buffer> buffer;

	/**
	 * The predicted buffer access method.
	 */
	Buffer::usage usage;
};

}} // openage::renderer

#endif
