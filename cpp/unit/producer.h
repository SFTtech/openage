// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_PRODUCER_H_
#define OPENAGE_UNIT_PRODUCER_H_

#include <memory>
#include <unordered_map>
#include <vector>


#include "../coord/tile.h"
#include "../gamedata/gamedata.gen.h"
#include "../gamedata/graphic.gen.h"

namespace openage {

class GameMain;
class Terrain;
class Texture;
class TestSound;

class Unit;
class UnitAbility;
class UnitAction;

/**
 * Initializes a unit with the required attributes, each unit type should implement these funcrtions
 * initialise should be called on construction of units 'new Unit(some_unit_producer)'
 * place is called to customise how the unit gets added to the world -- used to setup the TerrainObject position
 */
class UnitProducer {
public:
	virtual ~UnitProducer() {}

	/**
	 * Initialize units attributes
	 */
	virtual void initialise(Unit *) = 0;

	/**
	 * set unit in place -- return if placement was successful
	 */
	virtual bool place(Unit *, Terrain *, coord::tile) = 0;

	/**
	 * Get a default text for HUD drawing
	 */
	virtual Texture *default_texture() = 0;
};

/**
 * temporary class -- will be replaced with nyan system in future
 * Stores graphics and attributes for a single unit type
 */
class UnitTypeTest: public UnitProducer {
public:
	UnitTypeTest(const gamedata::unit_living *,
	             Texture *,
	             Texture *,
	             Texture *,
	             Texture *,
	             TestSound *,
	             TestSound *,
	             TestSound *,
	             TestSound *);

	virtual ~UnitTypeTest();

	void initialise(Unit *);
	bool place(Unit *, Terrain *, coord::tile);
	Texture *default_texture();

private:
	const gamedata::unit_living unit_data;
	Texture *terrain_outline;
	Texture *dead;
	Texture *idle;
	Texture *moving;
	Texture *attacking;
	TestSound *on_create;
	TestSound *on_destroy;
	TestSound *on_move;
	TestSound *on_attack;
};

/**
 * Stores graphics and attributes for a building type
 * Will be replaced with nyan system in future
 */
class BuldingProducer: public UnitProducer {
public:
	BuldingProducer(Texture *tex, coord::tile_delta foundation_size,
	                int foundation, TestSound *create,  TestSound *destroy);
	virtual ~BuldingProducer();

	/**
	 * Sound id played when object is created or destroyed.
	 */
	TestSound *on_create;
	TestSound *on_destroy;

	void initialise(Unit *);
	bool place(Unit *, Terrain *, coord::tile);
	Texture *default_texture();

private:
	Texture *terrain_outline;
	Texture *texture;
	coord::tile_delta size;
	int foundation_terrain;
};

class AssetManager;

/**
 * another temporary class to be later replaced with nyan
 */
class ProducerLoader {
public:
	ProducerLoader(GameMain *);

	/**
	 * makes producers for all types in the game data
	 */
	std::vector<std::unique_ptr<UnitProducer>> create_producers(const std::vector<gamedata::empiresdat> &gamedata, int your_civ_id);

	/**
	 * loads required assets to produce an entity type
	 * returns null if type cannot be created
	 */
	std::unique_ptr<UnitProducer> load_building(const gamedata::unit_building &);
	std::unique_ptr<UnitProducer> load_living(const gamedata::unit_living &);
	std::unique_ptr<UnitProducer> load_object(const gamedata::unit_object &);


private:
	GameMain *main;

};

} // namespace openage

#endif
