// Copyright 2015-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "resources/texture_data.h"


namespace openage {
namespace renderer {

/**
 * An abstract base for a handle to a texture buffer allocated in graphics hardware.
 * Can be obtained by passing texture data to the renderer.
 */
class Texture2d {
public:
	virtual ~Texture2d();

	/**
	 * Get the texture information.
	 *
	 * @return Information about the texture, such as size and format.
	 */
	const resources::Texture2dInfo &get_info() const;

	/**
	 * Copies this texture's data from graphics hardware into a CPU-accessible
	 * Texture2dData buffer.
	 *
	 * @return A Texture2dData object containing the texture data.
	 */
	virtual resources::Texture2dData into_data() = 0;

	/**
	 * Uploads the provided data into the GPU texture storage. The format has
	 * to match the format this Texture was originally created with.
	 *
	 * @param data The texture data to upload.
	 */
	virtual void upload(resources::Texture2dData const &) = 0;

protected:
	/**
	 * Constructs the base with the given information.
	 *
	 * @param info Information about the texture, such as size and format.
	 */
	Texture2d(const resources::Texture2dInfo &);

	/**
	 * Information about the size, format, etc. of this texture.
	 */
	resources::Texture2dInfo info;
};

} // namespace renderer
} // namespace openage
