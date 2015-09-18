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
	};

	Buffer(Context *ctx, size_t size=0);
	virtual ~Buffer() = default;

	Buffer(const Buffer &other) = delete;
	Buffer(Buffer &&other) = delete;
	Buffer &operator =(const Buffer &other) = delete;
	Buffer &operator =(Buffer &&other) = delete;

	/**
	 * Returns the raw pointer to the buffer memory area.
	 */
	char *get() const;

	/**
	 * Create an empty buffer of the given size.
	 */
	void create(size_t size);

	/**
	 * Returns the allocated buffer size.
	 */
	size_t size() const;

	/**
	 * Uploads the current state of the buffer to the GPU.
	 */
	virtual void upload(bind_target target) const = 0;

	/**
	 * The associated graphics context.
	 */
	Context *const context;

protected:
	/**
	 * The raw buffer containing the data.
	 */
	std::unique_ptr<char[]> buffer;

	/**
	 * Stores the allocated buffer size.
	 */
	size_t allocd;
};

}} // openage::renderer

#endif
