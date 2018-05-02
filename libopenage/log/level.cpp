// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "level.h"

namespace openage {

namespace log {

static constexpr level_value undefined {{"UNDEFINED", 999}, "5"};

level::level() : Enum{undefined} {}

}} // openage::log
