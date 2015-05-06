// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_TERRAIN_TERRAIN_OBJECT_H_
#define OPENAGE_TERRAIN_TERRAIN_OBJECT_H_

#include <memory>
#include <stddef.h>

#include "terrain.h"
#include "terrain_chunk.h"
#include "../pathfinding/path.h"
#include "../coord/tile.h"
#include "../coord/phys3.h"
#include "../texture.h"

namespace openage {

class Terrain;
class TerrainChunk;
class Texture;
class Unit;

/**
 * only placed will enable collision checks
 */
enum class object_state {
	removed,
	floating,
	placed,
	placed_no_collision
};

/**
 * A rectangle or square of tiles which is the minimim
 * space to fit the units foundation or radius
 * the end tile will have ne and se values greater or equal to
 * the start tile
 */
struct tile_range {
	coord::tile start;
	coord::tile end;	// start <= end
	coord::phys3 draw;	// gets used as center point of radial objects
};

/**
 * get all tiles in the tile range -- useful for iterating
 * returns a flat list of tiles between the rectangle enclosed
 * by the tile_range start and end tiles
 */
std::vector<coord::tile> tile_list(const tile_range &rng);

/**
 * given the west most point of a building foundation and the tile_delta
 * size of the foundation, this will return the tile range covered by the base,
 * which includes start and end tiles, and phys3 center point (used for drawing)
 */
tile_range building_center(coord::phys3 west, coord::tile_delta size);

/**
 * half a tile
 */
constexpr coord::phys3_delta phys_half_tile = coord::phys3_delta{
	coord::settings::phys_per_tile / 2,
	coord::settings::phys_per_tile / 2,
	0
};

/**
 * Base class for map location types which include square tile aligned
 * positions and radial positions This enables two inheriting classes
 * SquareObject and RadialObject to specify different areas of the map
 *
 * All TerrainObjects are owned by a Unit, to construct TerrainObjects,
 * use the make_location function on any Unit
 *
 * This class allows intersection testing between two TerrainObjects to
 * cover all cases of intersection (water, land or flying objects) the units
 * lambda function is used which takes a tile and returns a bool value of
 * whether that tile can be passed by this
 *
 * The name of this class is likely to change to TerrainBase or TerrainSpace
 */
class TerrainObject : public std::enable_shared_from_this<TerrainObject> {
public:
	TerrainObject(Unit &u);
	TerrainObject(const TerrainObject &) = delete;	// disable copy constructor
	TerrainObject(TerrainObject &&) = delete;		// disable move constructor
	virtual ~TerrainObject();

	/**
	 * the range of tiles which are covered by this object
	 */
	tile_range pos;

	/**
	 * unit which is inside this base
	 * used to find the unit from user actions
	 *
	 * every terrain object should contain a single unit
	 */
	Unit &unit;

	/**
	 * is the object a floating outline -- it is only an indicator
	 * of where a building will be built, but not yet started building
	 * and does not affect any collisions
	 */
	bool is_floating() const;

	/**
	 * has the object been placed
	 */
	bool is_placed() const;

	/**
	 * should this object be tested for collisions, arrows should not
	 */
	bool check_collisions() const;

	/**
	 * decide which terrains this object can be on
	 * this function should be true if given a valid position for the object
	 */
	std::function<bool(const coord::phys3 &)> passable;

	/**
	 * specifies content to be drawn
	 */
	std::function<void()> draw;

	/**
	 * draws outline of this terrain space in current position
	 */
	void draw_outline() const;

	/**
	 * upgrades a floating building to a placed state
	 */
	bool place(object_state init_state);

	/**
	 * binds the TerrainObject to a certain TerrainChunk.
	 *
	 * @param terrain: the terrain where the object will be placed onto.
	 * @param pos: (tile) position of the (nw,sw) corner
	 * @param init_state should be floating, placed or placed_no_collision
	 * @returns true when the object was placed, false when it did not fit at pos.
	 */
	bool place(std::shared_ptr<Terrain> t, coord::phys3 &pos, object_state init_state);

	/**
	 * moves the object -- returns false if object cannot be moved here
	 */
	bool move(coord::phys3 &pos);

	/**
	 * remove this TerrainObject from the terrain chunks.
	 */
	void remove();

	/**
	 * sets all the ground below the object to a terrain id.
	 *
	 * @param id: the terrain id to which the ground is set
	 * @param additional: amount of additional space arround the building
	 */
	void set_ground(int id, int additional=0);

	/**
	 * add a child terrain object
	 */
	void annex(TerrainObject *other);

	const TerrainObject *get_parent() const;

	std::vector<TerrainObject *> get_children() const;

	/*
	 * terrain this object was placed on
	 */
	std::shared_ptr<Terrain> get_terrain() const {
		return terrain.lock();
	}

	/**
	 * comparison for TerrainObjects.
	 *
	 * sorting for vertical placement.
	 * by using this order algorithm, the overlapping order
	 * is optimal so the objects can be drawn in correct order.
	 */
	bool operator <(const TerrainObject &other);

	/**
	 * returns the range of tiles covered if the object was in the given pos
	 * @param pos the position to find a range for
	 */
	virtual tile_range get_range(const coord::phys3 &pos) const = 0;

	/**
	 * how far is a point from the edge of this object
	 */
	virtual coord::phys_t from_edge(const coord::phys3 &point) const = 0;

	/**
	 * get a position on the edge of this object
	 */
	virtual coord::phys3 on_edge(const coord::phys3 &angle, coord::phys_t extra=0) const = 0;

	/**
	 * does this space contain a given point
	 */
	virtual bool contains(const coord::phys3 &other) const = 0;

	/**
	 * would this intersect with another object if it were positioned at the given point
	 */
	virtual bool intersects(const TerrainObject &other, const coord::phys3 &position) const = 0;

	/**
	 * the shortest line that can be placed across the objects center
	 */
	virtual coord::phys_t min_axis() const = 0;

protected:
	object_state state;

	std::weak_ptr<Terrain> terrain;
	int occupied_chunk_count;
	TerrainChunk *occupied_chunk[4];

	/**
	 * annexes and grouped units
	 */
	TerrainObject *parent;
	std::vector<TerrainObject *> children;

	/**
	 * texture for drawing outline
	 */
	std::shared_ptr<Texture> outline_texture;

	/**
	 * placement function which does not check passibility
	 * used only when passibilty is already checked
	 * otherwise the place function should be used
	 * this does not modify the units placement state
	 */
	void place_unchecked(std::shared_ptr<Terrain> t, coord::phys3 &position);
};

/**
 * terrain object class represents one immobile object on the map (building, trees, fish, ...).
 * can only be constructed by unit->make_location<SquareObject>(...)
 */
class SquareObject: public TerrainObject {

public:
	virtual ~SquareObject();


	/**
	 * tile size of this objects base
	 */
	const coord::tile_delta size;

	/**
	 * calculate object start and end positions.
	 *
	 * @param pos: the center position of the building
	 *
	 * set the center position to "middle",
	 * start_pos is % and end_pos = &
	 *
	 * for a building, the # tile will be "the clicked one":
	 *           @              @           @
	 *         @   @          @   @      %#   &
	 *       @   @   @      %   #   &       @
	 *     %   #   @   &      @   @
	 *       @   @   @          @
	 *         @   @
	 *           @
	 */
	tile_range get_range(const coord::phys3 &pos) const override;

	coord::phys_t from_edge(const coord::phys3 &point) const override;
	coord::phys3 on_edge(const coord::phys3 &angle, coord::phys_t extra=0) const override;
	bool contains(const coord::phys3 &other) const override;
	bool intersects(const TerrainObject &other, const coord::phys3 &position) const override;
	coord::phys_t min_axis() const override;

private:
	SquareObject(Unit &u, coord::tile_delta foundation_size);
	SquareObject(Unit &u, coord::tile_delta foundation_size, std::shared_ptr<Texture> out_tex);

	friend class Unit;
};

/**
 * Represents circular shaped objects (movable game units)
 * can only be constructed by unit->make_location<RadialObject>(...)
 */
class RadialObject: public TerrainObject {
public:
	virtual ~RadialObject();

	/**
	 * radius of this cirular space
	 */
	const coord::phys_t phys_radius;

	/**
	 * finds the range covered if the object was in a position
	 */
	tile_range get_range(const coord::phys3 &pos) const override;

	coord::phys_t from_edge(const coord::phys3 &point) const override;
	coord::phys3 on_edge(const coord::phys3 &angle, coord::phys_t extra=0) const override;
	bool contains(const coord::phys3 &other) const override;
	bool intersects(const TerrainObject &other, const coord::phys3 &position) const override;
	coord::phys_t min_axis() const override;

private:
	RadialObject(Unit &u, float rad);
	RadialObject(Unit &u, float rad, std::shared_ptr<Texture> out_tex);

	friend class Unit;
};

} //namespace openage

#endif
