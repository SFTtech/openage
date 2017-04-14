// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "texture_info.h"
#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

/// Stores CPU-accessible texture data in a byte buffer. Can be loaded and stored from/onto disk
/// and passed to or read from graphics hardware. In a sense it provides a mediator between
/// the filesystem, the CPU and graphics hardware.
class TextureData {
public:
	/// Create a texture from an image file.
	/// @param[in] use_metafile determines whether the loading should read an accompanying
	///                         metadata file to split the texture into subtextures
	///
	/// Uses SDL Image internally. For supported image file types,
	/// see the SDL_Image initialization in the engine.
	TextureData(const util::Path &path, bool use_metafile = false);

	/// Construct by moving the information and raw texture data from somewhere else.
	TextureData(TextureInfo &&info, std::vector<uint8_t> &&data);

	/// Returns the information describing this texture data.
	const TextureInfo& get_info() const;

	/// Returns a pointer to the raw texture data.
	const uint8_t *get_data() const;

	/// Stores this texture data in the given file in the PNG format.
	void store(const util::Path& file) const;

private:
	/// Information about this texture data.
	TextureInfo info;

	/// The raw texture data.
	std::vector<uint8_t> data;
};

}}} // namespace openage::renderer::resources
