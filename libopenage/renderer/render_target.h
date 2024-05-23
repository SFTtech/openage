// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>


namespace openage {
namespace renderer {
class Texture2d;

namespace resources {
class Texture2dData;
} // namespace resources

/// The abstract base for a render target.
class RenderTarget : public std::enable_shared_from_this<RenderTarget> {
protected:
	RenderTarget() = default;

public:
	virtual ~RenderTarget() = default;

	/**
	 * Get an image from the pixels in the render target's framebuffer.
	 *
	 * This should only be called _after_ rendering to the framebuffer has finished.
	 *
	 * @return RGBA texture data.
	 */
	virtual resources::Texture2dData into_data() = 0;

	virtual std::vector<std::shared_ptr<Texture2d>> get_texture_targets() = 0;
};

} // namespace renderer
} // namespace openage
