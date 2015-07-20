// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GENERATOR_H_
#define OPENAGE_GENERATOR_H_

#include "rng/rng.h"
#include "assetmanager.h"
#include "options.h"

namespace openage {

class GameSpec;


using tileset_t = std::unordered_set<coord::tile>;

coord::tile random_tile(rng::RNG &rng, tileset_t tiles);

class Region {
public:
	Region(int size);
	Region(coord::tile center, tileset_t tiles);

	tileset_t get_tiles() const;

	coord::tile get_center() const;

	coord::tile get_tile(rng::RNG &rng) const;

	tileset_t subset(rng::RNG &rng, coord::tile tile, unsigned int number) const;

	Region take_tiles(rng::RNG &rng, coord::tile tile, unsigned int number);

	Region take_random(rng::RNG &rng, unsigned int number);

	int owner;
	int object_id;
	int terrain_id;

private:
	coord::tile center;
	tileset_t tiles;

};






/**
 * Manages creation and setup of new games
 *
 * required values used to construct a game
 * this includes game spec and players
 *
 * this will be identical for each networked
 * player in a game
 */
class Generator : public options::OptionNode {
public:
	Generator(Engine *engine);

	/**
	 * game spec used by this generator
	 */
	std::shared_ptr<GameSpec> get_spec() const;

	/**
	 * return the list of player names
	 */
	std::vector<std::string> player_names() const;

	/**
	 * returns the generated terrain
	 */
	std::shared_ptr<Terrain> terrain() const;

	/**
	 * places all initial objects
	 */
	void add_units(GameMain &m) const;

	void create();

private:
	void create_regions();

	// access to games asset files
	AssetManager assetmanager;

	// data version used to create a game
	std::shared_ptr<GameSpec> spec;

	// the generated data
	std::vector<Region> regions;

};

} // namespace openage

#endif
