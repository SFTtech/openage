// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include "../engine.h"
#include "resource.h"
#include "json/json.h"

namespace openage {

	class Condition {
		public:
			Condition();
			virtual ~Condition() {};

			/*
			 * check if condition is true
			 */
			virtual bool check(uint32_t gametime,uint32_t update) = 0;

			/*
			 * nearly every condition need the gamestate
			 */
			openage::GameMain* game;

			/*
			 * json interface
			 */
			virtual Json::Value toJson() = 0;

	};
}
