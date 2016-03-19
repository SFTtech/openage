#pragma once

#include <cstdint>
#include <vector>

#include "../engine.h"
#include "trigger.h"

namespace openage {

	class Triggers : public TickHandler
	{
	public:
	Triggers();
	~Triggers();

	void addTrigger(Trigger *trigger);
	void reset();
	bool on_tick();

	private:
		uint32_t gametime;
		// offset for gamestart and pauses
		uint32_t offset;
		// trigger container
		std::vector<Trigger*> triggers;
	};
}
