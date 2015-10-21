// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_VERTEX_STATE_H_
#define OPENAGE_RENDERER_VERTEX_STATE_H_

#include <memory>
#include <unordered_set>

namespace openage {
namespace renderer {

class Buffer;
class Context;
class VertexBuffer;


/**
 * Represents a vertex buffer state.
 * You can enable this vertex state context so that the
 * GPU pipeline uses the state defined in here for the vertex specification.
 * This class is specialized by each graphics backend.
 */
class VertexState {
public:
	VertexState(Context *ctx);

	const VertexState &operator =(VertexState &&other) = delete;
	const VertexState &operator =(const VertexState &other) = delete;
	VertexState(const VertexState &other) = delete;
	VertexState(VertexState &&other) = delete;

	virtual ~VertexState() = default;

	/**
	 * Attach the given vertex buffer to this vertex state.
	 * Buffer contents are assigned to their corresponding
	 * attribute layout ids.
	 *
	 * This just enables the buffer to be used when this vertex
	 * state is active.
	 */
	virtual void attach_buffer(VertexBuffer &buf) = 0;

	/**
	 * Remove the attributes of the given vertex buffer from the
	 * active list.
	 */
	virtual void detach_buffer(VertexBuffer &buf) = 0;

	/**
	 * Make this vertex state the current one.
	 */
	virtual void bind() const = 0;

protected:
	/**
	 * Associated rendering context
	 */
	Context *const context;

	/**
	 * This vertex state has data for attributes with
	 * layout ids stored in here.
	 */
	std::unordered_set<int> bound_attributes;
};

}} // openage::renderer

#endif
