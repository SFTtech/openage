// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "game_main.h"
#include "json/json.h"

namespace openage {

	class Action {
		public:
			Action() {};
			virtual ~Action() {};

			/*
			 * this will be executed to change gamestate
			 */
			virtual void execute() = 0;

			/*
			 * json interface
			 */
			virtual Json::Value toJson() = 0;
	};

}
