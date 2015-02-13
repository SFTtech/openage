// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_PRODUCER_H_
#define OPENAGE_UNIT_PRODUCER_H_

#include <memory>
#include <unordered_map>
#include <vector>


#include "../coord/tile.h"
#include "../gamedata/gamedata.gen.h"
#include "../gamedata/graphic.gen.h"
#include "unit.h"

namespace openage {

class DataManager;
class GameMain;
class Terrain;
class TerrainObject;
class Texture;
class Sound;

class UnitAbility;
class UnitAction;
class UnitTexture;

/**
 * Initializes a unit with the required attributes, each unit type should implement these funcrtions
 * initialise should be called on construction of units 'new Unit(some_unit_producer)'
 * place is called to customise how the unit gets added to the world -- used to setup the TerrainObject position
 */
class UnitProducer {
public:
	virtual ~UnitProducer() {}

	/**
	 * gets the id of the unit type being produced
	 */
	virtual int producer_id() = 0;

	/**
	 * Initialize units attributes
	 * This can be called using existing units to modify type
	 */
	virtual void initialise(Unit *) = 0;

	/**
	 * set unit in place -- return if placement was successful
	 *
	 * This should be used when initially creating a unit or
	 * when a unit is ungarrsioned from a building or object
	 */
	virtual TerrainObject *place(Unit *, Terrain *, coord::phys3) = 0;

	/**
	 * Get a default texture for HUD drawing
	 */
	virtual UnitTexture *default_texture() = 0;

	/**
	 * all instances of units made from this producer
	 * this could allow all units of a type to be upgraded
	 */
	std::vector<UnitReference> instances;

	/**
	 * The set of graphics used for this type
	 */
	graphic_set graphics;
};

/**
 * base game data unit type
 */
class ObjectProducer: public UnitProducer {
public:
	ObjectProducer(DataManager &dm, const gamedata::unit_object *ud);
	virtual ~ObjectProducer();

	int producer_id();
	void initialise(Unit *);
	TerrainObject *place(Unit *, Terrain *, coord::phys3);
	UnitTexture *default_texture();

protected:
	DataManager &datamanager;
	const gamedata::unit_object unit_data;

	/**
	 * Sound id played when object is created or destroyed.
	 */
	Sound *on_create;
	Sound *on_destroy;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> default_tex;
	UnitProducer *dead_unit_producer;
	coord::tile_delta foundation_size;
};

/**
 * temporary class -- will be replaced with nyan system in future
 * Stores graphics and attributes for a single unit type
 * in aoe living units are derived from objects
 */
class LivingProducer: public ObjectProducer {
public:
	LivingProducer(DataManager &dm, const gamedata::unit_living *);
	virtual ~LivingProducer();

	void initialise(Unit *);
	TerrainObject *place(Unit *, Terrain *, coord::phys3);

private:
	const gamedata::unit_living unit_data;
	UnitTexture *moving;
	UnitTexture *attacking;
	Sound *on_move;
	Sound *on_attack;
	UnitProducer *projectile;

};

/**
 * Stores graphics and attributes for a building type
 * Will be replaced with nyan system in future
 * in aoe buildings are derived from living units
 */
class BuldingProducer: public UnitProducer {
public:
	BuldingProducer(DataManager &dm, const gamedata::unit_building *ud);
	virtual ~BuldingProducer();

	int producer_id();
	void initialise(Unit *);
	TerrainObject *place(Unit *, Terrain *, coord::phys3);
	UnitTexture *default_texture();

private:
	DataManager &datamanager;
	const gamedata::unit_building unit_data;

	/**
	 * Sound id played when object is created or destroyed.
	 */
	Sound *on_create;
	Sound *on_destroy;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> texture;
	std::shared_ptr<UnitTexture> destroyed;
	UnitProducer *trainable1;
	UnitProducer *trainable2;
	UnitProducer *projectile;
	coord::tile_delta foundation_size;
	int foundation_terrain;

	TerrainObject *make_annex(int annex_id, Terrain *t, coord::phys3 annex_pos, bool c);
};

class ProjectileProducer: public UnitProducer {
public:
	ProjectileProducer(DataManager &dm, const gamedata::unit_projectile *);

	virtual ~ProjectileProducer();

	int producer_id();
	void initialise(Unit *);
	TerrainObject *place(Unit *, Terrain *, coord::phys3);
	UnitTexture *default_texture();

private:
	const gamedata::unit_projectile unit_data;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> tex;
	std::shared_ptr<UnitTexture> sh; // shadow texture
	std::shared_ptr<UnitTexture> destroyed;
};

} // namespace openage

#endif
