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

	/// Reads the pixel at the given position and casts it to the given type.
	/// The texture is _not_ read as if it consisted of pixels of the given type,
	/// but rather according to its original pixel format.
	template<typename T>
	T read_pixel(size_t x, size_t y) const {
		const uint8_t *data = this->data.data();
		auto dims = this->info.get_size();
		size_t off = (dims.second - y) * this->info.get_row_size();
		off += x * pixel_size(this->info.get_format());

		if ((off + sizeof(T)) > this->info.get_data_size()) {
			throw Error(MSG(err) << "Pixel position (" << x << ", " << y << ") is outside texture.");
		}

		return *reinterpret_cast<const T*>(data + off);
	}

	/// Stores this texture data in the given file in the PNG format.
	void store(const util::Path& file) const;

private:
	/// Information about this texture data.
	TextureInfo info;

	/// The raw texture data.
	std::vector<uint8_t> data;
};

}}} // namespace openage::renderer::resources
