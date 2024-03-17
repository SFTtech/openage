// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "pathfinder.h"

#include "pathfinding/integrator.h"


namespace openage::path {

Pathfinder::Pathfinder() :
	grids{},
	integrator{std::make_shared<Integrator>()} {
}

const std::shared_ptr<Grid> &Pathfinder::get_grid(size_t idx) const {
	return this->grids.at(idx);
}

} // namespace openage::path
