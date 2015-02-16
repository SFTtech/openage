// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "type_pair.h"

namespace openage {

UnitType::UnitType() {}

bool UnitType::match(Unit *) {
	// TODO: types
	return true;
}

TypePair::TypePair() {}

} // namespace openage