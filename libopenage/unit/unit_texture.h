// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_UNIT_TEXTURE_H_
#define OPENAGE_UNIT_UNIT_TEXTURE_H_

#include <cstdint>
#include <vector>

#include "../coord/phys3.h"
#include "../gamedata/graphic.gen.h"

namespace openage {

class GameSpec;
class Texture;
class Sound;

/**
 * Handling animated and directional textures based on the game
 * graphics data.
 *
 * These objects handle the drawing of regular textures to use a
 * unit's direction and include delta graphics.
 *
 * This type can also deals with playing position based game sounds.
 */
class UnitTexture {
public:
	/**
	 * Delta option specifies whether the delta graphics are included.
	 *
	 * Note that the game data contains loops in delta links
	 * which mean recursive loading should be avoided
	 */
	UnitTexture(GameSpec &spec, uint16_t graphic_id, bool delta=true);
	UnitTexture(GameSpec &spec, const gamedata::graphic *graphic, bool delta=true);

	/**
	 * const attributes of the graphic
	 */
	const int16_t      id;
	const int16_t      sound_id;
	const unsigned int frame_count;
	const unsigned int angle_count;
	const int16_t      mirroring_mode;
	const float        frame_rate;

	/**
	 * draw object with vertial orientation (arrows)
	 * adding an addtion degree of orientation
	 */
	const bool         use_up_angles;

	/**
	 * use delta information
	 */
	const bool         use_deltas;

	/**
	 * invalid unit textures will cause errors if drawn
	 */
	bool is_valid() const;

	/**
	 * pixel size of this texture
	 */
	coord::window size() const;

	/**
	 * a sample drawing for hud
	 */
	void sample(const coord::camhud &draw_pos, unsigned color=1) const;

	/**
	 * draw object with no direction
	 */
	void draw(const coord::camgame &draw_pos, unsigned int frame, unsigned color) const;

	/**
	 * draw object with direction
	 */
	void draw(const coord::camgame &draw_pos, coord::phys3_delta &dir, unsigned int frame, unsigned color) const;

	/**
	 * initialise graphic data
	 */
	void initialise(GameSpec &spec);

private:
	/**
	 * use a regular texture for drawing
	 */
	const Texture *texture;

	/**
	 * the above frame count covers the entire graphic (with deltas)
	 * the actual number in the base texture may be different
	 */
	unsigned int safe_frame_count;
	unsigned int angles_included;
	unsigned int angles_mirrored;
	unsigned int top_frame;

	// avoid drawing missing graphics
	bool draw_this;
	Sound *sound;

	// delta graphic ids
	std::vector<gamedata::graphic_delta> delta_id;

	// delta graphics
	std::vector<std::pair<std::unique_ptr<UnitTexture>, coord::camgame_delta>> deltas;

	/**
	 * find which subtexture should be used for drawing this texture
	 */
	unsigned int subtexture(const Texture *t, unsigned int angle, unsigned int frame) const;
};

/**
 * the set of images to used based on unit direction,
 * usually 8 directions to draw for each unit (3 are mirrored)
 *
 * @param dir a world space direction,
 * @param angles number of angles, usually 8
 * @param first_angle offset added to angle, modulo number of angles
 * @return image set index
 */
unsigned int dir_group(coord::phys3_delta dir, unsigned int angles=8);

} // namespace openage

#endif
