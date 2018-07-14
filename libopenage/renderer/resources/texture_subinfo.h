// Copyright 2013-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include "../../util/csv.h"


namespace openage {
namespace renderer {
namespace resources {

/**
 * Describes a position of a (sub)texture within a larger texture.
 * Used for texture atlases and animations where all frames
 * are within one texture file. */
struct Texture2dSubInfo {
	/// The subtexture position within the atlas
	uint32_t x, y;

	/// The subtexture size
	uint32_t w, h;

	/// Position of the subtexture's center within the atlas
	uint32_t cx, cy;

	/// Initializes the info from a CSV line containing its members.
	Texture2dSubInfo(const std::string& line);

	/// Initializes the info from data.
	Texture2dSubInfo(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t cx, uint32_t cy);

	Texture2dSubInfo() = default;

	/// Needed for compatibility reasons, don't use this directly.
	int fill(const std::string& line);
};

}}}
