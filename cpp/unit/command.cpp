// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "command.h"

namespace openage {

Command::Command(Unit *) {}

Command::Command(coord::phys3) {}

Command::Command(UnitProducer *) {}

Command::Command(UnitProducer *, coord::phys3) {}

void Command::set_ability(ability_type) {}

} // namespace openage
