// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_SPEC_H_
#define OPENAGE_GAME_SPEC_H_

#include <unordered_map>

#include "job/job.h"
#include "gamedata/gamedata.gen.h"
#include "gamedata/graphic.gen.h"
#include "terrain/terrain.h"
#include "unit/unit_texture.h"
#include "util/timer.h"

namespace openage {

class AssetManager;
class UnitType;

/**
 * the key type mapped to data objects
 */
using index_t = int;
using unit_type_list = std::vector<std::unique_ptr<UnitType>>;

/**
 * simple sound object
 */
class Sound {
public:
	void play();

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
	GameSpec(AssetManager *am);
	virtual ~GameSpec();

	void check_updates();

	/**
	 * check if loading has been completed
	 */
	bool load_complete();

	/**
	 * return data used for constructing terrain objects
	 */
	terrain_meta *get_terrain_meta();

	/**
	 * number of producers available
	 */
	size_t producer_count();

	/**
	 * reverse lookup of slp
	 */
	index_t get_slp_graphic(index_t slp);

	/**
	 * get a texture by id, this specifically avoids returning the missing placeholder texture
	 */
	Texture *get_texture(index_t graphic_id);

	/**
	 * get unit texture by graphic id -- this is an directional texture
	 * and also includes graphic deltas
	 */
	std::shared_ptr<UnitTexture> get_unit_texture(index_t graphic_id);

	/**
	 * get sound by sound id
	 */
	Sound *get_sound(index_t sound_id);

	/**
	 * unit types by aoe gamedata unit ids -- the unit type which corresponds to an aoe unit id
	 */
	UnitType *get_type(index_t type_id);

	/**
	 * unit types by list index -- a continuous array of all types
	 */
	UnitType *get_type_index(size_t type_index);

	/**
	 * data for a graphic
	 */
	const gamedata::graphic *get_graphic_data(index_t grp_id);

	/**
	 * get game data for a building
	 */
	const gamedata::unit_building *get_building_data(index_t unit_id);

	/**
	 * get available commands for a unit id
	 */
	std::vector<const gamedata::unit_command *> get_command_data(index_t unit_id);


private:
	AssetManager *assetmanager;

	/**
	 * data used for constructing terrain objects
	 */
	terrain_meta terrain_data;

	/**
	 * all available game objects.
	 */
	unit_type_list available_objects;

	/**
	 * slp to graphic reverse lookup
	 */
	std::unordered_map<index_t, index_t> slp_to_graphic;

	/**
	 * map graphic id to gamedata graphic.
	 */
	std::unordered_map<index_t, const gamedata::graphic *> graphics;

	/**
	 * used for annex creation
	 */
	std::unordered_map<index_t, const gamedata::unit_building *> buildings;

	/**
	 * commands available for each unit id
	 */
	std::unordered_map<index_t, std::vector<const gamedata::unit_command *>> commands;

	/**
	 * unit ids -> unit type for that id
	 */
	std::unordered_map<index_t, UnitType *> producers;

	/**
	 * unit ids -> unit type for that id
	 */
	std::unordered_map<index_t, std::shared_ptr<UnitTexture>> unit_textures;

	/**
	 * all available sounds.
	 */
	std::unordered_map<index_t, Sound> available_sounds;

	/**
	 * begin the main loading job
	 */
	void initialize(AssetManager *am);

	/**
	 * check graphic id is valid
	 */
	bool valid_graphic_id(index_t);

	/**
	 * create unit abilities from game data
	 */
	void create_abilities(const std::vector<gamedata::empiresdat> &gamedata);

	/**
	 * makes producers for all types in the game data
	 */
	void create_unit_types(const std::vector<gamedata::empiresdat> &gamedata, int your_civ_id);

	/**
	 * loads required assets to construct a unit type
	 * adds to the type list if the object can be created safely
	 */
	void load_building(const gamedata::unit_building &, unit_type_list &);
	void load_living(const gamedata::unit_living &, unit_type_list &);
	void load_object(const gamedata::unit_object &, unit_type_list &);
	void load_projectile(const gamedata::unit_projectile &, unit_type_list &);

	/**
	 * fill in the terrain_data attribute of this
	 */
	void load_terrain(AssetManager *am);

	/**
	 * has game data been load yet
	 */
	bool gamedata_loaded;
	openage::job::Job<std::vector<gamedata::empiresdat>> gamedata_load_job;
	void on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata);
	util::Timer load_timer;
};

}

#endif
