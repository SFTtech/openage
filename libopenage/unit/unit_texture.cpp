// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "unit_texture.h"

#include <cmath>
#include <iostream>

#include "../coord/phys.h"
#include "../coord/pixel.h"
#include "../gamestate/old/game_spec.h"
#include "../log/log.h"
#include "../util/math.h"
#include "../util/math_constants.h"


namespace openage {

UnitTexture::UnitTexture(GameSpec &spec, uint16_t graphic_id, bool delta) :
	UnitTexture{spec, spec.get_graphic_data(graphic_id), delta} {}

UnitTexture::UnitTexture(GameSpec &spec, const gamedata::graphic *graphic, bool delta) :
	id{graphic->graphic_id},
	sound_id{graphic->sound_id},
	frame_count{graphic->frame_count},
	angle_count{graphic->angle_count},
	mirroring_mode{graphic->mirroring_mode},
	frame_rate{graphic->frame_rate},
	use_up_angles{graphic->mirroring_mode == 24},
	use_deltas{delta},
	draw_this{true},
	sound{nullptr},
	delta_id{graphic->graphic_deltas.data} {
	this->initialise(spec);
}

bool UnitTexture::is_valid() const {
	return false;
}

coord::viewport UnitTexture::size() const {
	return coord::viewport{0, 0};
}

void UnitTexture::sample(const coord::CoordManager &coord, const coord::camhud &draw_pos, unsigned color) const {
}

void UnitTexture::draw(const coord::CoordManager &coord, const coord::camgame &draw_pos, unsigned int frame, unsigned color) const {
}

void UnitTexture::draw(const coord::CoordManager &coord, const coord::camgame &draw_pos, coord::phys3_delta &dir, unsigned int frame, unsigned color) const {
}

void UnitTexture::initialise(GameSpec &spec) {
}

unsigned int dir_group(coord::phys3_delta dir, unsigned int angles) {
	unsigned int first_angle = 5 * angles / 8;

	// normalise direction vector
	double len = std::hypot(dir.ne, dir.se);
	double dir_ne = static_cast<double>(dir.ne) / len;
	double dir_se = static_cast<double>(dir.se) / len;

	// formula to find the correct angle
	return static_cast<unsigned int>(
			   round(angles * atan2(dir_se, dir_ne) * (math::INV_PI / 2))
			   + first_angle)
	       % angles;
}


} // namespace openage
