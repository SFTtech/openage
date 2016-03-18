#include <SDL2/SDL.h>

#include "triggers.h"

namespace openage {

	Triggers::Triggers() {

		// get game start time
		this->offset = SDL_GetTicks();
	}

	Triggers::~Triggers() {

	}

	bool Triggers::on_tick() {
		uint32_t gametime = SDL_GetTicks();

		// calculate last frametime
		uint32_t frametime = gametime - this->gametime;
		// calculate true gametime
		gametime -= this->offset;
		// save gametime
		this->gametime = gametime;

		// run through all registered triggers
		for(auto trigger : this->triggers) {
			if(trigger.isActivated and !trigger.isDeleted) {
				trigger.update(gametime,frametime);
			}
		}
		return true;
	}

	void Triggers::addTrigger(Trigger trigger)  {
		this->triggers.push_back(trigger);
	}

	void Triggers::reset() {
		this->triggers.clear();
	}

}
