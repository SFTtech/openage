// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "game_main.h"
#include "picojson.h"

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
			virtual picojson::value toJson() = 0;

			std::string getResourceString(game_resource e)
			  {
			    switch(e)
			    {
				case game_resource::food:  return "food";
				case game_resource::wood:  return "wood";
				case game_resource::gold:  return "gold";
				case game_resource::stone: return "stone";
				default: return("unkown");
			    }
			  }
	};

}
