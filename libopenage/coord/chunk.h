// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"

namespace openage {
namespace coord {

using chunk_t = int32_t;


struct chunk_delta;
struct chunk;
struct chunk3_delta;
struct chunk3;


struct chunk_delta : CoordNeSeRelative<chunk_t, chunk, chunk_delta> {
	using CoordNeSeRelative<chunk_t, chunk, chunk_delta>::CoordNeSeRelative;
};

struct chunk : CoordNeSeAbsolute<chunk_t, chunk, chunk_delta> {
	using CoordNeSeAbsolute<chunk_t, chunk, chunk_delta>::CoordNeSeAbsolute;
};

struct chunk3_delta : CoordNeSeUpRelative<chunk_t, chunk3, chunk3_delta> {
	using CoordNeSeUpRelative<chunk_t, chunk3, chunk3_delta>::CoordNeSeUpRelative;
};

struct chunk3 : CoordNeSeUpAbsolute<chunk_t, chunk3, chunk3_delta> {
	using CoordNeSeUpAbsolute<chunk_t, chunk3, chunk3_delta>::CoordNeSeUpAbsolute;
};


} // namespace coord
} // namespace openage
