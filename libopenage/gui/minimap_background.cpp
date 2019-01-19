// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "minimap_background.h"

#include <limits>

#include "../terrain/terrain.h"
#include "../gamedata/color.gen.h"
#include "../error/error.h"

namespace openage {
namespace gui {

namespace {
const int spacing = 1;
}

std::tuple<std::unique_ptr<uint32_t[]>, int, int> generate_minimap_background(Terrain &terrain, const std::vector<gamedata::palette_color> &palette) {
	auto bounding_rect = terrain.used_bounding_rect();
    auto origin = coord::tile::from_chunk(std::get<0>(bounding_rect), {0, 0});
    auto size = coord::tile_delta::from_chunk(std::get<1>(bounding_rect) - std::get<0>(bounding_rect), {0, 0});

	auto max_size = std::max(size.ne, size.se);

	ENSURE(max_size <= std::numeric_limits<int>::max() - spacing, "Map is too big for the minimap textures.");

	const int texture_size = max_size + spacing;

	/**
	 * Generate terrain texture.
	 */
	std::unique_ptr<uint32_t[]> pixels = std::make_unique<uint32_t[]>(texture_size * texture_size);

	for (auto row = 0; row < max_size; ++row) {
		for (auto column = 0; column < max_size; ++column) {
			/**
			 * Note that the texture is filled row by row.
			 * A row of pixels spans from sw to ne, column - nw to se.
			 * Each pixel represents a tile.
			 */
			coord::tile tile_pos = origin + coord::tile_delta{column, row};

			TileContent *tile_data = terrain.get_data(tile_pos);

			if (tile_data) {
				uint8_t pal_idx = terrain.map_color_hi(tile_data->terrain_id);
				gamedata::palette_color hi_color = palette[pal_idx];
				pixels[row * texture_size + column] = 0xff000000 | hi_color.b << 16 | hi_color.g << 8 | hi_color.r;
			}
		}
	}

	return std::make_tuple(std::move(pixels), texture_size, spacing);
}

size_t terrain_used_chunks_hash(const Terrain &terrain) {
	return terrain.used_chunks_hash();
}

std::tuple<coord::chunk, coord::chunk_t> terrain_bounding_square(const Terrain &terrain) {
	auto rect = terrain.used_bounding_rect();
	auto size = std::get<1>(rect) - std::get<0>(rect);
	auto max_side_len = std::max(size.ne, size.se);
	return std::make_tuple(std::get<0>(rect), max_side_len);
}

coord::chunk_t terrain_square_dimensions(const std::tuple<coord::chunk, coord::chunk_t> &square) {
	return std::get<1>(square);
}

coord::tile terrain_square_center(const std::tuple<coord::chunk, coord::chunk_t> &square) {
	auto side_len = std::get<1>(square);

    static_assert(coord::tiles_per_chunk % 2 == 0, "Center of a even-sided bounding rectangle will be a fractional numbers of tiles.");
    const coord::tile_delta pos_on_tile({side_len/2, side_len/2});
    return coord::tile::from_chunk(std::get<0>(square), pos_on_tile);
}

}} // namespace openage::gui
