// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_PRODUCER_H_
#define OPENAGE_UNIT_PRODUCER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/tile.h"
#include "../gamedata/gamedata.gen.h"
#include "../gamedata/graphic.gen.h"
#include "../player.h"
#include "unit.h"
#include "unit_type.h"

namespace openage {

class GameMain;
class GameSpec;
class Terrain;
class TerrainObject;
class Texture;
class Sound;

class UnitAbility;
class UnitAction;
class UnitTexture;


std::unordered_set<terrain_t> allowed_terrains(const gamedata::ground_type &restriction);

/**
 * base game data unit type
 */
class ObjectProducer: public UnitType {
public:
	ObjectProducer(GameSpec &spec, const gamedata::unit_object *ud);
	virtual ~ObjectProducer();

	int id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

protected:
	GameSpec &dataspec;
	const gamedata::unit_object unit_data;

	/**
	 * decaying objects have a timed lifespan
	 */
	bool decay;

	/**
	 * Sound id played when object is created or destroyed.
	 */
	Sound *on_create;
	Sound *on_destroy;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> default_tex;
	UnitType *dead_unit_producer;

};

/**
 * movable unit types
 */
class MovableProducer: public ObjectProducer {
public:
	MovableProducer(GameSpec &spec, const gamedata::unit_movable *);
	virtual ~MovableProducer();

	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

protected:
	const gamedata::unit_movable unit_data;
	UnitTexture *moving;
	UnitTexture *attacking;
	Sound *on_move;
	Sound *on_attack;
	UnitType *projectile;

};

/**
 * temporary class -- will be replaced with nyan system in future
 * Stores graphics and attributes for a single unit type
 * in aoe living units are derived from objects
 */
class LivingProducer: public MovableProducer {
public:
	LivingProducer(GameSpec &spec, const gamedata::unit_living *);
	virtual ~LivingProducer();

	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

private:
	const gamedata::unit_living unit_data;

};

/**
 * Stores graphics and attributes for a building type
 * Will be replaced with nyan system in future
 * in aoe buildings are derived from living units
 */
class BuildingProducer: public UnitType {
public:
	BuildingProducer(GameSpec &spec, const gamedata::unit_building *ud);
	virtual ~BuildingProducer();

	int id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

private:
	GameSpec &dataspec;
	const gamedata::unit_building unit_data;

	/**
	 * Sound id played when object is created or destroyed.
	 */
	Sound *on_create;
	Sound *on_destroy;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> texture;
	std::shared_ptr<UnitTexture> destroyed;
	UnitType *trainable1;
	UnitType *trainable2;
	UnitType *projectile;
	int foundation_terrain;

	/**
	 * used for objects like town centers or gates
	 * where the base does not apply collision checks
	 */
	bool enable_collisions;

	TerrainObject *make_annex(Unit &u, std::shared_ptr<Terrain> t, int annex_id, coord::phys3 annex_pos, bool c) const;
};

/**
 * creates projectiles
 * todo use MovableProducer as base class
 */
class ProjectileProducer: public UnitType {
public:
	ProjectileProducer(GameSpec &spec, const gamedata::unit_projectile *);
	virtual ~ProjectileProducer();

	int id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

private:
	const gamedata::unit_projectile unit_data;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> tex;
	std::shared_ptr<UnitTexture> sh; // shadow texture
	std::shared_ptr<UnitTexture> destroyed;
};

} // namespace openage

#endif
