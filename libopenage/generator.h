// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GENERATOR_H_
#define OPENAGE_GENERATOR_H_

#include "rng/rng.h"
#include "assetmanager.h"
#include "options.h"

namespace openage {

class GameSpec;

/**
 * the type to store a set of tiles
 */
using tileset_t = std::unordered_set<coord::tile>;

/**
 * picks a random tile from a set
 */
coord::tile random_tile(rng::RNG &rng, tileset_t tiles);

/**
 * the four directions available for 2d tiles
 */
constexpr coord::tile_delta const neigh_tiles[] = {
	{ 1,  0},
	{-1,  0},
	{ 0,  1},
	{ 0, -1}
};

/**
 * A region is a set of tiles around a starting point,
 * including functions to create child regions
 */
class Region {
public:
	/**
	 * a square of tiles ranging from
	 * {-size, -size} to {size, size}
	 */
	Region(int size);

	/**
	 * a specified set of tiles
	 */
	Region(coord::tile center, tileset_t tiles);

	/**
	 * all tiles in this region
	 */
	tileset_t get_tiles() const;

	/**
	 * the center point of the region
	 */
	coord::tile get_center() const;

	/**
	 * picks a random tile from this subset
	 */
	coord::tile get_tile(rng::RNG &rng) const;

	/**
	 * find a group of tiles inside this region, number is the number of tiles to be contained
	 * in the subset, p is a probability between 0.0 and 1.0 which produces various shapes. a value
	 * of 1.0 produces circular shapes, where as a low value produces more scattered shapes. a value
	 * of 0.0 should not be used, and will always return no tiles
	 */
	tileset_t subset(rng::RNG &rng, coord::tile start_tile, unsigned int number, double p) const;

	/**
	 * removes the given set of tiles from this region, which get split of as a new child region
	 */
	Region take_tiles(rng::RNG &rng, coord::tile start_tile, unsigned int number, double p);

	/**
	 * similiar to take_tiles, but takes a random group of tiles
	 */
	Region take_random(rng::RNG &rng, unsigned int number, double p);

	/**
	 * player id of the owner, 0 for none
	 */
	int owner;

	/**
	 * the object to be placed on each tile of this region
	 * 0 for placing no object
	 */
	int object_id;

	/**
	 * the base terrain for this region
	 */
	int terrain_id;

private:

	/**
	 * the center tile of this region
	 */
	coord::tile center;

	/**
	 * tiles in this region
	 */
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

	bool create();

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
