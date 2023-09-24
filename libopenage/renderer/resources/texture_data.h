// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "texture_info.h"


namespace openage {
namespace util {
class Path;
}
namespace renderer::resources {

/// Stores 2D texture data in a CPU-accessible byte buffer. Provides methods for loading from
/// and storing onto disk, as well as sending to and receiving from graphics hardware.
class Texture2dData {
public:
	/// Create a texture from an image file.
	/// @param path Path to the image file.
	///
	/// Uses QImage internally.
	Texture2dData(const util::Path &path);

	/// Create a texture from info.
	///
	/// Uses QImage internally. For supported image file types,
	/// see the QImage initialization in the engine.
	Texture2dData(Texture2dInfo const &info);

	/// Construct by moving the information and raw texture data from somewhere else.
	Texture2dData(Texture2dInfo const &info, std::vector<uint8_t> &&data);

	/// Flips the texture along the Y-axis and returns the flipped data with the same info.
	/// Sometimes necessary when converting between storage modes.
	Texture2dData flip_y();

	/// Returns the information describing this texture data.
	const Texture2dInfo &get_info() const;

	/// Returns a pointer to the raw texture data, in row-major order.
	const uint8_t *get_data() const;

	/// Reads the pixel at the given position and casts it to the given type.
	/// The texture is _not_ read as if it consisted of pixels of the given type,
	/// but rather according to its original pixel format, so the coordinates
	/// have to be specified according to that.
	template <typename T>
	T read_pixel(size_t x, size_t y) const {
		const uint8_t *data = this->data.data();
		auto dims = this->info.get_size();
		size_t off = (dims.second - y - 1) * this->info.get_row_size();
		off += x * pixel_size(this->info.get_format());

		if ((off + sizeof(T)) > this->info.get_data_size()) {
			throw Error(MSG(err) << "Pixel position (" << x << ", " << y << ") is outside texture.");
		}

		return *reinterpret_cast<const T *>(data + off);
	}

	/// Stores this texture data in the given file in the PNG format.
	void store(const util::Path &file) const;

private:
	/// Information about this texture data.
	Texture2dInfo info;

	/// The raw texture data.
	std::vector<uint8_t> data;
};

} // namespace renderer::resources
} // namespace openage
