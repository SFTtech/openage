// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "condition_timer_loop.h"

namespace openage {

	ConditionTimerLoop::ConditionTimerLoop(uint32_t ms) {
		this->ms = ms;
	}

	ConditionTimerLoop::~ConditionTimerLoop() {

	}

	bool ConditionTimerLoop::check(uint32_t gametime,uint32_t update) {
		if( gametime - this->last > this->ms ) {
			// needed if check is not called every tick
			while(gametime - this->last > this->ms) {
				this->last += this->ms;
			}
			return true;
		}
		return false;
	}

	picojson::value ConditionTimerLoop::toJson() {
		picojson::object json;
		json["type"]  = picojson::value("timer-loop");
		json["value"] = picojson::value( (double) this->ms );
		return picojson::value(json);
	}
}
