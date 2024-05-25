// Copyright 2016-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"
#include "declarations.h"

namespace openage {
namespace coord {


struct chunk_delta : CoordNeSeRelative<chunk_t, chunk, chunk_delta> {
	using CoordNeSeRelative<chunk_t, chunk, chunk_delta>::CoordNeSeRelative;

	tile_delta to_tile(tile_t tiles_per_chunk) const;
};

struct chunk : CoordNeSeAbsolute<chunk_t, chunk, chunk_delta> {
	using CoordNeSeAbsolute<chunk_t, chunk, chunk_delta>::CoordNeSeAbsolute;

	tile to_tile(tile_t tiles_per_chunk) const;
};

struct chunk3_delta : CoordNeSeUpRelative<chunk_t, chunk3, chunk3_delta> {
	using CoordNeSeUpRelative<chunk_t, chunk3, chunk3_delta>::CoordNeSeUpRelative;
};

struct chunk3 : CoordNeSeUpAbsolute<chunk_t, chunk3, chunk3_delta> {
	using CoordNeSeUpAbsolute<chunk_t, chunk3, chunk3_delta>::CoordNeSeUpAbsolute;
};


} // namespace coord
} // namespace openage
