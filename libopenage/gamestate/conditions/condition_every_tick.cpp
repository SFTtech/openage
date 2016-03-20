// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "condition_every_tick.h"

namespace openage {

	ConditionEveryTick::ConditionEveryTick() {
	}

	ConditionEveryTick::~ConditionEveryTick() {

	}

	bool ConditionEveryTick::check(uint32_t gametime,uint32_t update) {
		return true;
	}

	picojson::value ConditionEveryTick::toJson() {
		picojson::object json;
		json["type"] = picojson::value("every-tick");
		return picojson::value(json);
	}
}
