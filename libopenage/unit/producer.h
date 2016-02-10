// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

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
	ObjectProducer(const Player &owner, const GameSpec &spec, const gamedata::unit_object *ud);
	virtual ~ObjectProducer();

	int id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

protected:
	const GameSpec &dataspec;
	const gamedata::unit_object unit_data;

	/**
	 * decaying objects have a timed lifespan
	 */
	bool decay;

	/**
	 * Sound id played when object is created or destroyed.
	 */
	const Sound *on_create;
	const Sound *on_destroy;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> default_tex;
	int dead_unit_id;

};

/**
 * movable unit types
 */
class MovableProducer: public ObjectProducer {
public:
	MovableProducer(const Player &owner, const GameSpec &spec, const gamedata::unit_movable *);
	virtual ~MovableProducer();

	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

protected:
	const gamedata::unit_movable unit_data;
	UnitTexture *moving;
	UnitTexture *attacking;
	const Sound *on_move;
	const Sound *on_attack;
	int projectile;

};

/**
 * temporary class -- will be replaced with nyan system in future
 * Stores graphics and attributes for a single unit type
 * in aoe living units are derived from objects
 */
class LivingProducer: public MovableProducer {
public:
	LivingProducer(const Player &owner, const GameSpec &spec, const gamedata::unit_living *);
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
	BuildingProducer(const Player &owner, const GameSpec &spec, const gamedata::unit_building *ud);
	virtual ~BuildingProducer();

	int id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

private:
	const GameSpec &dataspec;
	const gamedata::unit_building unit_data;

	/**
	 * Sound id played when object is created or destroyed.
	 */
	const Sound *on_create;
	const Sound *on_destroy;
	std::shared_ptr<Texture> terrain_outline;
	std::shared_ptr<UnitTexture> texture;
	std::shared_ptr<UnitTexture> destroyed;
	int trainable1;
	int trainable2;
	int projectile;
	int foundation_terrain;
	std::vector<game_resource> get_accepted_resources();

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
	ProjectileProducer(const Player &owner, const GameSpec &spec, const gamedata::unit_projectile *);
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
