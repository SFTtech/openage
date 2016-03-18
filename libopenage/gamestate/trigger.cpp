#include "trigger.h"

namespace openage {

	Trigger::Trigger() {

	}

	Trigger::~Trigger() {
		// remover all actions and conditions, since they are references
		for(auto action : this->actions) {
			//delete(action);
		}
		for(auto condition : this->conditions) {
			//delete(condition);
		}
	}

	void Trigger::update(uint32_t gametime, uint32_t update) {
		if(this->check(gametime,update)) {
			for(auto action : this->actions) {
				action->execute();
			}
		}
	}

	bool Trigger::check(uint32_t gametime, uint32_t update) {
		// and gate => everything must be true
		if( this->gate == Gate::AND ) {
			for(auto condition : this->conditions) {
				if( condition->check(gametime,update) == false) {
					return false;
				}
			}
			return true;
		}
		// or gate, one must be true
		if( this->gate == Gate::OR ) {
			for(auto condition : this->conditions) {
				if( condition->check(gametime,update) == true) {
					return true;
				}
			}
			return false;
		}
		// xor gate => exactly one condition is true
		uint8_t trues = 0;
		if( this->gate == Gate::XOR ) {
			for(auto condition : this->conditions) {
				if( condition->check(gametime,update) == true) {
					trues++;
					if( trues > 1) {
						return false;
					}
				}
			}
			if( trues == 1) { return true; } else { return false; }
		}
		// default case
		return false;
	}
}
