// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../condition.h"
#include "../game_main.h"
#include "../resource.h"

namespace openage {

	class ConditionMinRessources : public virtual Condition{
		public:
			ConditionMinRessources(uint32_t player,game_resource resource, float value);
			~ConditionMinRessources();

			bool check(uint32_t gametime,uint32_t update);
			Json::Value toJson();

			uint32_t player        = 0;
			game_resource resource = game_resource::food;
			float value            = 0;

	};

}
