// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>


namespace openage::renderer {

/**
 * IDs for shader uniforms.
 */
using uniform_id_t = uint32_t;

/**
 * Graphics API types.
 */
enum class graphics_api_t {
	DEFAULT,
	OPENGL,
	VULKAN,
};


} // namespace openage::renderer
