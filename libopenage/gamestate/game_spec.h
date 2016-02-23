// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>

#include "../job/job.h"
#include "../gamedata/gamedata.gen.h"
#include "../gamedata/graphic.gen.h"
#include "../terrain/terrain.h"
#include "../unit/unit_texture.h"
#include "../util/timer.h"

namespace openage {

class AssetManager;
class UnitType;
class UnitTypeMeta;
class Player;

/**
 * the key type mapped to data objects
 */
using index_t = int;

/**
 * could use unique ptr
 */
using unit_type_list = std::vector<std::shared_ptr<UnitType>>;
using unit_meta_list = std::vector<std::shared_ptr<UnitTypeMeta>>;

/**
 * simple sound object
 * TODO: move to assetmanager
 */
class Sound {
public:
	void play() const;

	std::vector<int> sound_items;
};

/**
 * GameSpec gives a collection of all game elements
 * this currently includes unit types and terrain types
 * This provides a system which can easily allow game modding
 *
 * uses the AssetManager to gather
 * graphic data, composite textures and sounds.
 *
 * all types are sorted and stored by id values,
 * each data object is referenced by a type and id pair
 *
 * dealing directly with files done by asset manager
 * TODO: should the audio loading should be moved there?
 */
class GameSpec {
public:
	GameSpec(AssetManager &am);
	virtual ~GameSpec();

	/**
	 * Check if loading has been completed,
	 * a load percent would be nice
	 */
	bool load_complete() const;

	/**
	 * return data used for constructing terrain objects
	 */
	terrain_meta *get_terrain_meta();

	/**
	 * reverse lookup of slp
	 */
	index_t get_slp_graphic(index_t slp);

	/**
	 * lookup using a texture id, this specifically avoids returning the missing placeholder texture
	 */
	Texture *get_texture(index_t graphic_id) const;

	/**
	 * get unit texture by graphic id -- this is an directional texture
	 * which also includes graphic deltas
	 */
	std::shared_ptr<UnitTexture> get_unit_texture(index_t graphic_id) const;

	/**
	 * get sound by sound id
	 */
	const Sound *get_sound(index_t sound_id) const;

	/**
	 * gamedata for a graphic
	 * nyan will have to replace this somehow
	 */
	const gamedata::graphic *get_graphic_data(index_t grp_id) const;

	/**
	 * get available commands for a unit id
	 * nyan will have to replace this somehow
	 */
	std::vector<const gamedata::unit_command *> get_command_data(index_t unit_id) const;

	/**
	 * returns the name of a civ by index
	 */
	std::string get_civ_name(int civ_id) const;

	/**
	 * makes initial unit types for a particular civ id
	 */
	void create_unit_types(unit_meta_list &objects, int civ_id) const;

private:
	AssetManager *assetmanager;

	/**
	 * the used file paths
	 */
	std::string data_path;
	std::string graphics_path;
	std::string terrain_path;
	std::string blend_path;
	std::string sound_path;

	/**
	 * complete gamedata
	 */
	std::vector<gamedata::empiresdat> gamedata;

	/**
	 * data used for constructing terrain objects
	 */
	terrain_meta terrain_data;

	/**
	 * slp to graphic id reverse lookup
	 */
	std::unordered_map<index_t, index_t> slp_to_graphic;

	/**
	 * map graphic id to gamedata graphic.
	 */
	std::unordered_map<index_t, const gamedata::graphic *> graphics;

	/**
	 * commands available for each unit id
	 */
	std::unordered_map<index_t, std::vector<const gamedata::unit_command *>> commands;


	/**
	 * graphic ids -> unit texture for that id
	 */
	std::unordered_map<index_t, std::shared_ptr<UnitTexture>> unit_textures;

	/**
	 * sound ids mapped to playable sounds for all available sounds.
	 */
	std::unordered_map<index_t, Sound> available_sounds;

	/**
	 * begin the main loading job
	 */
	void initialize(AssetManager &am);

	/**
	 * check graphic id is valid
	 */
	bool valid_graphic_id(index_t) const;

	/**
	 * create unit abilities from game data
	 */
	void create_abilities(const std::vector<gamedata::empiresdat> &gamedata);

	/**
	 * loads required assets to construct a unit type
	 * adds to the type list if the object can be created safely
	 */
	void load_building(const gamedata::unit_building &, unit_meta_list &) const;
	void load_living(const gamedata::unit_living &, unit_meta_list &) const;
	void load_object(const gamedata::unit_object &, unit_meta_list &) const;
	void load_projectile(const gamedata::unit_projectile &, unit_meta_list &) const;

	/**
	 * fill in the terrain_data attribute of this
	 */
	void load_terrain(AssetManager &am);

	/**
	 * has game data been load yet
	 */
	bool gamedata_loaded;
	openage::job::Job<bool> gamedata_load_job;
	void on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata);
	util::Timer load_timer;
};

}
