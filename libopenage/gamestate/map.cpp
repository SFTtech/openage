// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "map.h"

#include "gamestate/terrain.h"
#include "pathfinding/pathfinder.h"


namespace openage::gamestate {
Map::Map(const std::shared_ptr<Terrain> &terrain) :
	terrain{terrain},
	pathfinder{std::make_shared<path::Pathfinder>()} {
	// TODO: Initialize pathfinder with terrain path costs
}

Map::Map(const std::shared_ptr<Terrain> &terrain,
         const std::shared_ptr<path::Pathfinder> &pathfinder) :
	terrain{terrain},
	pathfinder{pathfinder} {
	// TODO: Initialize pathfinder with terrain path costs
}

const util::Vector2s &Map::get_size() const {
	return this->terrain->get_size();
}

const std::shared_ptr<Terrain> &Map::get_terrain() const {
	return this->terrain;
}

const std::shared_ptr<path::Pathfinder> &Map::get_pathfinder() const {
	return this->pathfinder;
}

} // namespace openage::gamestate
