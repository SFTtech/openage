// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_BUFFER_H_
#define OPENAGE_RENDERER_BUFFER_H_

#include <memory>

namespace openage {
namespace renderer {

class Context;


/**
 * Context-specialized graphics system buffer.
 */
class Buffer {
public:
	enum class bind_target {
		vertex_attributes,
		element_indices,
		query,
	};

	enum class usage {
		static_draw,
		static_read,
		stream_draw,
		stream_read,
		stream_copy,
		static_copy,
		dynamic_draw,
		dynamic_read,
		dynamic_copy,
	};

  std::shared_ptr<int> wow;
	Buffer(/*Context *ctx,*/ size_t size=0);
	virtual ~Buffer() = default;

	Buffer(const Buffer &other) = delete;
	Buffer(Buffer &&other) = delete;
	Buffer &operator =(const Buffer &other) = delete;
	Buffer &operator =(Buffer &&other) = delete;

	/**
	 * Returns the raw pointer to the buffer memory area.
	 */
	char *get(bool mark_dirty=false);

	/**
	 * Mark the buffer as changed so it will be reuploaded.
	 */
	void mark_dirty();

	/**
	 * Create an empty buffer of the given size.
	 */
	void create(size_t size);

	/**
	 * Returns the allocated buffer size.
	 */
	size_t size() const;

	/**
	 * Uploads the current buffer contents to the GPU.
	 */
	virtual void upload(bind_target target, usage usage) = 0;

	/**
	 * Bind this buffer to the given target.
	 */
	virtual void bind(bind_target target) const = 0;

	/**
	 * The associated graphics context.
	 */
	//Context *const context;

protected:
	/**
	 * The raw buffer containing the data.
	 */
	std::unique_ptr<char[]> buffer;

	/**
	 * Stores the allocated buffer size.
	 */
	size_t allocd;

	/**
	 * True, when the buffer is present on the GPU.
	 */
	bool on_gpu;
};

}} // openage::renderer

#endif
