// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../action.h"
#include "../resource.h"

namespace openage {

	class ActionAddResource  : public virtual Action{
		public:
			ActionAddResource(float amount,uint16_t player,game_resource resource);
			ActionAddResource(Json::Value action);
			~ActionAddResource();

			void execute();
			Json::Value toJson();

			float    amount = 0;
			uint16_t player = 0;
			game_resource resource;
			openage::GameMain* game;
	};

}
