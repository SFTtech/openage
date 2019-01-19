// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../job/job.h"
#include "../gamedata/gamedata.gen.h"
#include "../gamedata/graphic.gen.h"
#include "../terrain/terrain.h"
#include "../unit/unit_texture.h"
#include "../util/csv.h"

#include <unordered_map>
#include <memory>
#include <QObject>


namespace openage {

class AssetManager;
class GameSpec;
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
	Sound(GameSpec *spec, std::vector<int> &&sound_items)
		:
		sound_items{sound_items},
		game_spec{spec} {}

	void play() const;

	std::vector<int> sound_items;

	GameSpec *game_spec;
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

	/**
	 * perform the main loading job.
	 * this loads all the data into the storage.
	 */
	bool initialize();

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
	 * lookup using a texture file name
	 */
	Texture *get_texture(const std::string &file_name, bool use_metafile=true) const;

	/**
	 * get unit texture by graphic id -- this is an directional texture
	 * which also includes graphic deltas
	 */
	std::shared_ptr<UnitTexture> get_unit_texture(index_t graphic_id) const;

	/**
	 * get a palette by filename
	 *
	 * @param name: the asset file name relative to the asset root.
	 * @returns color array.
	 */
	std::vector<gamedata::palette_color> get_palette(const std::string &name) const;

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

	/**
	 * Return the asset manager used for loading resources
	 * of this game specification.
	 */
	AssetManager *get_asset_manager() const;

private:
	/**
	 * check graphic id is valid
	 */
	bool valid_graphic_id(index_t) const;

	/**
	 * create unit abilities from game data
	 */
	void create_abilities(const gamedata::empiresdat &gamedata);

	/**
	 * loads required assets to construct a buildings.
	 * adds to the type list if the object can be created safely.
	 */
	void load_building(const gamedata::building_unit &, unit_meta_list &) const;

	/**
	 * loads assets for living things.
	 */
	void load_living(const gamedata::living_unit &, unit_meta_list &) const;

	/**
	 * load assets for other game objects (not building and living).
	 */
	void load_object(const gamedata::unit_object &, unit_meta_list &) const;

	/**
	 * load missile assets.
	 */
	void load_missile(const gamedata::missile_unit &, unit_meta_list &) const;

	/**
	 * fill in the terrain_data attribute of this
	 */
	void load_terrain(const gamedata::empiresdat &gamedata);

	/**
	 * Invoked when the gamedata has been loaded.
	 */
	void on_gamedata_loaded(const gamedata::empiresdat &gamedata);

	/**
	 * Asset management entity that is responsible for textures, sounds, etc.
	 */
	AssetManager *assetmanager;

	/**
	 * The full original gamedata tree.
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
	 * has game data been load yet
	 */
	bool gamedata_loaded;
};

} // openage

namespace qtsdl {
class GuiItemLink;
} // qtsdl

namespace openage {

class GameSpecSignals;

/**
 * Game specification instanciated in QML.
 * Linked to the "GameSpec" QML type.
 *
 * Wraps the "GameSpec" C++ class from above.
 */
class GameSpecHandle {
public:
	explicit GameSpecHandle(qtsdl::GuiItemLink *gui_link);

	/**
	 * Control whether this specification can be loaded (=true)
	 * or will not be loaded (=false).
	 */
	void set_active(bool active);

	/**
	 * invoked from qml when the asset_manager member is set.
	 */
	void set_asset_manager(AssetManager *asset_manager);

	/**
	 * Return if the specification was fully loaded.
	 */
	bool is_ready() const;

	/**
	 * forget everything about the specification and
	 * reload it with `start_loading_if_needed`.
	 */
	void invalidate();

	/**
	 * signal about a loaded spec if any
	 */
	void announce_spec();

	/**
	 * Return the contained game specification.
	 */
	std::shared_ptr<GameSpec> get_spec();

private:
	/**
	 * load the game specification if not already present.
	 */
	void start_loading_if_needed();

	/**
	 * Actually dispatch the loading job to the job manager.
	 */
	void start_load_job();

	/**
	 * called from the job manager when the loading job finished.
	 */
	void on_loaded(job::result_function_t<bool> result);

	/**
	 * The real game specification.
	 */
	std::shared_ptr<GameSpec> spec;

	/**
	 * enables the loading of the game specification.
	 */
	bool active;

	AssetManager *asset_manager;

public:
	std::shared_ptr<GameSpecSignals> gui_signals;
	qtsdl::GuiItemLink *gui_link;
};

class GameSpecSignals : public QObject {
	Q_OBJECT

public:
signals:
	/*
	 * Some load job has finished.
	 *
	 * To be sure that the latest result is used, do the verification at the point of use.
	 */
	void load_job_finished();

	void game_spec_loaded(std::shared_ptr<GameSpec> loaded_game_spec);
};

}
