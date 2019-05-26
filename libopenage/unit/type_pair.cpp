// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "type_pair.h"

namespace openage {

UnitType::UnitType() {}

bool UnitType::match(Unit *) {
	// \todo add types
	return true;
}

TypePair::TypePair() {}

} // namespace openage
