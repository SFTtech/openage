// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cmath>
#include <iostream>

#include "../coord/phys3.h"
#include "../crossplatform/math_constants.h"
#include "../datamanager.h"
#include "../texture.h"
#include "unit_texture.h"

namespace openage {

UnitTexture::UnitTexture(DataManager *dm, uint16_t graphic_id, bool delta)
	:
	UnitTexture{dm, dm->get_graphic_data(graphic_id), delta} {}

UnitTexture::UnitTexture(DataManager *dm, const gamedata::graphic *graphic, bool delta)
	:
	id{graphic->id},
	frame_count{graphic->frame_count},
	angle_count{graphic->angle_count},
	mirroring_mode{graphic->mirroring_mode},
	frame_rate{graphic->frame_rate},
	use_up_angles{graphic->mirroring_mode == 24},
	draw_this{true},
	texture{dm->get_texture(graphic->id)},
	sound{dm->get_sound(graphic->sound_id)} {
	if (not is_valid()) {
		this->draw_this = false;
	}

	// find deltas
	if (delta) for (auto d : graphic->graphic_deltas.data) {
		if (dm->get_graphic_data(d.graphic_id)) {
			UnitTexture *ut = new UnitTexture(dm, d.graphic_id, false);
			if (ut->is_valid()) {
				this->deltas.push_back({ut, coord::camgame_delta{d.direction_x, d.direction_y}});
			}
		}
	}
	
	if (this->draw_this) {

		// the graphic frame count includes deltas
		unsigned int subtextures = this->texture->get_subtexture_count();
		log::dbg("subtexture count %u, frame count %u", subtextures, this->frame_count);
		if (subtextures >= graphic->frame_count) {
			this->angles_included = subtextures / graphic->frame_count;
			this->angles_mirrored = this->angle_count - this->angles_included;
			this->safe_frame_count = graphic->frame_count;
		}
		else{
			this->angles_included = 1;
			this->angles_mirrored = 0;
			this->safe_frame_count = subtextures;
		}

		// find the top direction for mirroring over
		this->top_frame = this->angle_count - (1 - (this->angles_included - this->angles_mirrored) / 2);
	}
}

bool UnitTexture::is_valid() {
	return texture;
}

void UnitTexture::sample(const coord::camhud &draw_pos) {

	// draw delta list first
	for (auto d : this->deltas) {
		coord::camhud_delta dlt = coord::camhud_delta{d.second.x, d.second.y};
		d.first->sample(draw_pos + dlt);
	}

	// draw texture
	if (this->draw_this) {
		this->texture->draw(draw_pos, PLAYERCOLORED, false, 0, 1);
	}
}

void UnitTexture::draw(const coord::camgame &draw_pos, unsigned int frame, unsigned color) {

	// draw delta list first
	for (auto d : this->deltas) {
		d.first->draw(draw_pos + d.second, frame, color);
	}

	// draw texture
	if (this->draw_this) {
		unsigned int to_draw = this->subtexture(this->texture, 0, frame);
		this->texture->draw(draw_pos, PLAYERCOLORED, false, to_draw, color);
	}
}

void UnitTexture::draw(const coord::camgame &draw_pos, coord::phys3_delta &dir, unsigned int frame, unsigned color) {
	unsigned int frame_to_use = frame;
	if (this->use_up_angles) {
		// up  1 => tilt 0
		// up -1 => tilt 1
		// up has a scale 5 times smaller
		double len = sqrt(dir.ne*dir.ne + dir.se*dir.se + dir.up*dir.up/25);
		double up = static_cast<double>(dir.up/5.0) / len;
		frame_to_use = (0.5 - (0.5 * up)) * this->frame_count;
	}
	else if (this->sound && frame == 0.0) {
		this->sound->play();
	}

	// the index for the current direction
	unsigned int angle = dir_group(dir, this->angle_count);

	/*
	 * mirroring is used to make additional image sets
	 */
	bool mirror = false;
	if (this->angles_included <= angle) {
		// this->angles_included <= angle < this->angle_count
		angle = this->top_frame - angle;
		mirror = true;
	}

	// draw delta list first
	for (auto d : this->deltas) {
		 d.first->draw(draw_pos + d.second, dir, frame, color);
	}

	if (this->draw_this) {
		unsigned int to_draw = this->subtexture(this->texture, angle, frame_to_use);
		this->texture->draw(draw_pos, PLAYERCOLORED, mirror, to_draw, color);
	}
}

unsigned int UnitTexture::subtexture(Texture *t, unsigned int angle, unsigned int frame) {
	unsigned int tex_frames = t->get_subtexture_count();
	unsigned int count = tex_frames / this->angles_included;
	unsigned int to_draw = angle * count + (frame % count);
	if (tex_frames <= to_draw) {
		log::dbg("subtexure out of range %u %u", angle, frame);
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


} /* namespace openage */
