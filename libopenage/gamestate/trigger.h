#pragma once

#include <vector>
#include <cstdint>

#include "condition.h"
#include "conditions/condition_every_tick.h"
#include "conditions/condition_max_ressources.h"
#include "conditions/condition_min_ressources.h"
#include "conditions/condition_timer_loop.h"
#include "action.h"
#include "actions/action_add_gold.h"
#include "actions/action_add_wood.h"
#include "actions/action_add_food.h"
#include "actions/action_add_stone.h"

namespace openage {

	class Trigger
	{
		public:
			Trigger();
			~Trigger();

			enum class Gate { AND, OR, XOR };

			void update(uint32_t gametime,uint32_t update);

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
