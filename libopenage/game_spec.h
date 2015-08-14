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
	GameSpec(AssetManager &am);
	virtual ~GameSpec();

	/**
	 * Check if loading has been completed,
	 * a load percent would be nice
	 */
	bool load_complete();

	/**
	 * return data used for constructing terrain objects
	 */
	terrain_meta *get_terrain_meta();

	/**
	 * total number of unit types available
	 */
	size_t producer_count();

	/**
	 * reverse lookup of slp
	 */
	index_t get_slp_graphic(index_t slp);

	/**
	 * lookup using a texture id, this specifically avoids returning the missing placeholder texture
	 */
	Texture *get_texture(index_t graphic_id);

	/**
	 * get unit texture by graphic id -- this is an directional texture
	 * which also includes graphic deltas
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
	 * return all types in a particular named category
	 */
	std::vector<index_t> get_category(const std::string &c) const;

	/**
	 * return all used categories, such as living, building or projectile
	 */
	std::vector<std::string> get_type_categories() const;

	/**
	 * unit types by list index -- a continuous array of all types
	 * probably not a useful function / can be removed
	 */
	UnitType *get_type_index(size_t type_index);

	/**
	 * gamedata for a graphic
	 * nyan will have to replace this somehow
	 */
	const gamedata::graphic *get_graphic_data(index_t grp_id);

	/**
	 * gamedata for a building
	 * nyan will have to replace this somehow
	 */
	const gamedata::unit_building *get_building_data(index_t unit_id);

	/**
	 * get available commands for a unit id
	 * nyan will have to replace this somehow
	 */
	std::vector<const gamedata::unit_command *> get_command_data(index_t unit_id);


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
	 * all available game objects.
	 */
	unit_type_list available_objects;

	/**
	 * unit ids -> unit type for that id
	 */
	std::unordered_map<index_t, UnitType *> producers;

	/**
	 * all available categories of units
	 */
	std::vector<std::string> all_categories;

	/**
	 * category lists
	 */
	std::unordered_map<std::string, std::vector<index_t>> categories;


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
	std::unordered_map<index_t, std::shared_ptr<UnitTexture>> unit_textures;

	/**
	 * all available sounds.
	 */
	std::unordered_map<index_t, Sound> available_sounds;

	/**
	 * begin the main loading job
	 */
	void initialize(AssetManager &am);

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
	 * creates and adds items to categories
	 */
	void add_to_category(const std::string &c, index_t type);

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

#endif
