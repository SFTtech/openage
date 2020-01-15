// Copyright 2015-2020 the openage authors. See copying.md for legal info.

#include "unit_texture.h"

#include <cmath>
#include <iostream>

#include "../coord/phys.h"
#include "../coord/pixel.h"
#include "../gamestate/game_spec.h"
#include "../log/log.h"
#include "../texture.h"
#include "../util/math.h"
#include "../util/math_constants.h"


namespace openage {

UnitTexture::UnitTexture(GameSpec &spec, uint16_t graphic_id, bool delta)
	:
	UnitTexture{spec, spec.get_graphic_data(graphic_id), delta} {}

UnitTexture::UnitTexture(GameSpec &spec, const gamedata::graphic *graphic, bool delta)
	:
	id{graphic->graphic_id},
	sound_id{graphic->sound_id},
	frame_count{graphic->frame_count},
	angle_count{graphic->angle_count},
	mirroring_mode{graphic->mirroring_mode},
	frame_rate{graphic->frame_rate},
	use_up_angles{graphic->mirroring_mode == 24},
	use_deltas{delta},
	texture{nullptr},
	draw_this{true},
	sound{nullptr},
	delta_id{graphic->graphic_deltas.data} {
	this->initialise(spec);
}

bool UnitTexture::is_valid() const {
	return texture;
}

coord::viewport UnitTexture::size() const {
	return coord::viewport{this->texture->w, this->texture->h};
}

void UnitTexture::sample(const coord::CoordManager &coord, const coord::camhud &draw_pos, unsigned color) const {

	// draw delta list first
	for (auto &d : this->deltas) {
		coord::camhud_delta dlt = coord::camhud_delta{d.second.x, d.second.y};
		d.first->sample(coord, draw_pos + dlt, color);
	}

	// draw texture
	if (this->draw_this) {
		this->texture->draw(coord, draw_pos, PLAYERCOLORED, false, 0, color);
	}
}

void UnitTexture::draw(const coord::CoordManager &coord, const coord::camgame &draw_pos, unsigned int frame, unsigned color) const {

	// draw delta list first
	for (auto &d : this->deltas) {
		d.first->draw(coord, draw_pos + d.second, frame, color);
	}

	// draw texture
	if (this->draw_this) {
		unsigned int to_draw = frame % this->texture->get_subtexture_count();
		this->texture->draw(coord, draw_pos, PLAYERCOLORED, false, to_draw, color);
	}
}

void UnitTexture::draw(const coord::CoordManager &coord, const coord::camgame &draw_pos, coord::phys3_delta &dir, unsigned int frame, unsigned color) const {
	unsigned int frame_to_use = frame;
	if (this->use_up_angles) {
		// up  1 => tilt 0
		// up -1 => tilt 1
		// up has a scale 5 times smaller
		double len = math::hypot3(dir.ne.to_double(), dir.se.to_double(), dir.up.to_double()/5);
		double up = dir.up.to_double()/(5.0 * len);
		frame_to_use = (0.5 - (0.5 * up)) * this->frame_count;
	}
	else if (this->sound && frame == 0.0) {
		this->sound->play();
	}

	// the index for the current direction
	unsigned int angle = dir_group(dir, this->angle_count);

	// mirroring is used to make additional image sets
	bool mirror = false;
	if (this->angles_included <= angle) {
		// this->angles_included <= angle < this->angle_count
		angle = this->top_frame - angle;
		mirror = true;
	}

	// draw delta list first
	for (auto &d : this->deltas) {
		d.first->draw(coord, draw_pos + d.second, dir, frame, color);
	}

	if (this->draw_this) {
		unsigned int to_draw = this->subtexture(this->texture, angle, frame_to_use);
		this->texture->draw(coord, draw_pos, PLAYERCOLORED, mirror, to_draw, color);
	}
}

void UnitTexture::initialise(GameSpec &spec) {
	this->texture = spec.get_texture(this->id);
	this->sound = spec.get_sound(this->sound_id);
	if (not is_valid()) {
		this->draw_this = false;
	}

	// find deltas
	if (this->use_deltas) for (auto d : this->delta_id) {
		if (spec.get_graphic_data(d.graphic_id)) {
			auto ut = std::make_unique<UnitTexture>(spec, d.graphic_id, false);
			if (ut->is_valid()) {
				this->deltas.push_back({std::move(ut), coord::camgame_delta{d.offset_x, d.offset_y}});
			}
		}
	}

	if (this->draw_this) {

		// the graphic frame count includes deltas
		unsigned int subtextures = this->texture->get_subtexture_count();
		if (subtextures >= this->frame_count) {

			// angles with graphic data
			this->angles_included = subtextures / this->frame_count;
			this->angles_mirrored = this->angle_count - this->angles_included;
			this->safe_frame_count = this->frame_count;
		}
		else {
			this->angles_included = 1;
			this->angles_mirrored = 0;
			this->safe_frame_count = subtextures;
		}

		// find the top direction for mirroring over
		this->top_frame = this->angle_count - (1 - (this->angles_included - this->angles_mirrored) / 2);
	}
}

unsigned int UnitTexture::subtexture(const Texture *t, unsigned int angle, unsigned int frame) const {
	unsigned int tex_frames = t->get_subtexture_count();
	unsigned int count = tex_frames / this->angles_included;
	unsigned int to_draw = angle * count + (frame % count);
	if (tex_frames <= to_draw) {
		log::log(MSG(err) << "Subtexture out of range (" << angle << ", " << frame << ")");
		return 0;
	}
	return to_draw;
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
		+ first_angle
	) % angles;
}


} // namespace openage
