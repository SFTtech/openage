// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "trigger.h"

namespace openage {

	Trigger::Trigger() {

	}

	Trigger::~Trigger() {
		// remove all actions and conditions, since they are references
		for(auto action : this->actions) {
			delete(action);
		}
		this->actions.clear();

		for(auto condition : this->conditions) {
			delete(condition);
		}
		this->conditions.clear();
	}

	Trigger::Trigger(Json::Value trigger) {
		log::log(MSG(dbg) << "loading trigger");
		this->id = trigger.get("id",0).asInt64();
		this->isActivated = trigger.get("active",true).asBool();

		// gate
		std::string gate = trigger.get("gate","or").asString();
		this->gate = Trigger::Gate::OR;
		if( gate.compare("and") == 0 ) {
			this->gate = Trigger::Gate::AND;
		} else if ( gate.compare("xor") == 0) {
			this->gate = Trigger::Gate::XOR;
		}

		// load actions
		Action* a;
		for (auto action : trigger["actions"]) {
			std::string type = action.get("type","unknown").asString();
			if( type.compare("add-resource") == 0) {
				a = new ActionAddResource(action);
			}
			if(type.compare("unknown") != 0) {
				this->actions.push_back(a);
			}
		}

		// load conditions
		Condition* c;
		for (auto condition : trigger["conditions"]) {
			std::string type = condition.get("type","unknown").asString();
			if( type.compare("min-resources") == 0) {
				c = new ConditionMinRessources(condition);
			} else if( type.compare("max-resources") == 0) {
				c = new ConditionMaxRessources(condition);
			} else if( type.compare("timer-loop") == 0) {
				c = new ConditionTimerLoop(condition);
			} else if( type.compare("every-tick") == 0) {
				c = new ConditionEveryTick(condition);
			}
			if(type.compare("unknown") != 0) {
			  std::cout << "new condition";
				this->conditions.push_back(c);
			}
		}
	}

	std::string Trigger::getGateString() {
		switch(this->gate) {
		  case Gate::OR:
			return "or";
			break;
		  case Gate::AND:
			return "and";
			break;
		  case Gate::XOR:
			return "xor";
			break;
		}
		return "unkown";
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

	/*
	 * for Savefile
	 */
	Json::Value Trigger::toJson() {
		Json::Value triggerj;

		triggerj["id"]      = this->id;
		triggerj["gate"]    = this->getGateString();
		triggerj["active"]  = this->isActivated;
		triggerj["deleted"] = this->isDeleted;

		// save actions
		Json::Value actions;
		for(auto action : this->actions) {
			actions.append( action->toJson() );
		}
		// free triggers
		triggerj["actions"] = actions;

		// save conditions
		Json::Value conditions;
		for(auto condition : this->conditions) {
			conditions.append( condition->toJson() );
		}
		triggerj["conditions"] = conditions;

		return triggerj;
	}
}
