// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_DECL_H_
#define OPENAGE_COORD_DECL_H_

#include <stdint.h>

#include "fixed_point.h"

/*
 * forward declarations of all coordinate structs,
 * scalar type name aliases, and scaling factors
 *
 * in accordance to doc/implementation/coordinate-systems
 */

namespace openage {
namespace coord {

/* physics-based vector types */
using phys_t = FixedPoint<int64_t, 16>;

struct phys2;
struct phys2_delta;
struct phys3;
struct phys3_delta;

using tile_t = int64_t;

struct tile;
struct tile_delta;
struct tile3;
struct tile3_delta;

using chunk_t = int32_t;

struct chunk;
struct chunk_delta;

/* pixel-based vector types */
using pixel_t = int32_t;

struct window;
struct window_delta;
struct camgame;
struct camgame_delta;
struct camhud;
struct camhud_delta;

/* terminal character position-based types */
using term_t = int;

struct term;
struct term_delta;

/* general-purpose vector types */
struct vec2;
struct vec2_delta;
struct vec3;
struct vec3_delta;

struct vec2f;
struct vec2f_delta;
struct vec3f;
struct vec3f_delta;

} // namespace coord
} // namespace openage

#endif
