// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

/**
 * This file contains forward declarations for all the coordinate types,
 * as well as their scalar types.
 *
 * It was created to escape circular-include hell.
 */

#include <cstdint>

#include "util/fixed_point.h"

namespace openage {
namespace coord {

// forward declarations of phys.h types
constexpr unsigned int phys_t_radix_pos = 16;

using phys_t = util::FixedPoint<int64_t, phys_t_radix_pos>;

struct phys2_delta;
struct phys2;
struct phys3_delta;
struct phys3;

using phys_angle_t = util::FixedPoint<int32_t, 16>;

// forward declarations of scene.h types
constexpr unsigned int scene_t_radix_pos = 16;

using scene_t = util::FixedPoint<int64_t, scene_t_radix_pos>;

struct scene2_delta;
struct scene2;
struct scene3_delta;
struct scene3;

// forward declarations of tile.h types
using tile_t = int64_t;

constexpr tile_t tiles_per_chunk = 16;

struct tile_delta;
struct tile;
struct tile3_delta;
struct tile3;

// forward declarations of pixel.h types
using pixel_t = int32_t;

struct camgame_delta;
struct camgame;
struct viewport_delta;
struct viewport;
struct camhud_delta;
struct camhud;
struct input_delta;
struct input;

// forward declarations of chunk.h types
using chunk_t = int32_t;

struct chunk_delta;
struct chunk;
struct chunk3_delta;
struct chunk3;

// forward declarations of term.h types
using term_t = int;

struct term_delta;
struct term;

// forward declaration of the coord manager
class CoordManager;

} // namespace coord
} // namespace openage
