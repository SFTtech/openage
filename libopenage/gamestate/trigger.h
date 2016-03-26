// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>

#include "condition.h"
#include "conditions/condition_every_tick.h"
#include "conditions/condition_max_ressources.h"
#include "conditions/condition_min_ressources.h"
#include "conditions/condition_timer_loop.h"
#include "action.h"
#include "actions/action_add_resource.h"

namespace openage {

	class Trigger
	{
		public:
			Trigger();
			Trigger(Json::Value);
			~Trigger();

			enum class Gate { AND, OR, XOR };

			void update(uint32_t gametime,uint32_t update);

			/*
			 * painful :-(, is there a better solution for enum to string?
			 */
			std::string getGateString();

			/*
			 * for savefile
			 */
			Json::Value toJson();

			uint32_t id      = 0;
			bool isActivated = true;
			bool isDeleted   = false;
			Gate gate        = Gate::OR;
			std::vector<Condition*> conditions;
			std::vector<Action*>    actions;
		private:
			bool check(uint32_t gametime,uint32_t update);
	};
}
