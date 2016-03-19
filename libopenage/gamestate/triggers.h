// Copyright 2015-2016 the openage authors. See copying.md for legal info.

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

	static Triggers *getInstance();

	void addTrigger(Trigger *trigger);
	void reset();
	bool on_tick();

	private:
		uint32_t gametime;
		// offset for gamestart and pauses
		uint32_t offset;
		// trigger container
		std::vector<Trigger*> triggers;
		// Singleton
		static Triggers *instance;
	};
}
