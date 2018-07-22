// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "texture_info.h"
#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

/// Stores 2D texture data in a CPU-accessible byte buffer. Provides methods for loading from
/// and storing onto disk, as well as sending to and receiving from graphics hardware.
class Texture2dData {
public:
	/// Create a texture from an image file.
	/// @param[in] use_metafile determines whether the loading should read an accompanying
	///                         metadata file to split the texture into subtextures
	///
	/// Uses SDL Image internally. For supported image file types,
	/// see the SDL_Image initialization in the engine.
	Texture2dData(const util::Path &path, bool use_metafile = false);

	/// Construct by moving the information and raw texture data from somewhere else.
	Texture2dData(Texture2dInfo &&info, std::vector<uint8_t> &&data);

	/// Flips the texture along the Y-axis and returns the flipped data with the same info.
	/// Sometimes necessary when converting between storage modes.
	Texture2dData flip_y();

	/// Returns the information describing this texture data.
	const Texture2dInfo& get_info() const;

	/// Returns a pointer to the raw texture data, in row-major order.
	const uint8_t *get_data() const;

	/// Reads the pixel at the given position and casts it to the given type.
	/// The texture is _not_ read as if it consisted of pixels of the given type,
	/// but rather according to its original pixel format, so the coordinates
	/// have to be specified according to that.
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
	Texture2dInfo info;

	/// The raw texture data.
	std::vector<uint8_t> data;
};

}}} // namespace openage::renderer::resources
