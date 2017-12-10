// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <functional>

#include "../cvar/cvar.h"
#include "action.h"
#include "input_manager.h"
#include "../log/log.h"
#include "../log/message.h"
#include "../util/repr.h"


namespace openage {
namespace input {

namespace {


// the list of action names that will be added during the constructor of ActionManager.
const std::vector<std::string> DEFAULT_ACTIONS = {
	"START_GAME",
	"STOP_GAME",
	"TOGGLE_HUD",
	"SCREENSHOT",
	"TOGGLE_DEBUG_OVERLAY",
	"TOGGLE_DEBUG_GRID",
	"QUICK_SAVE",
	"QUICK_LOAD",
	"TOGGLE_MODE",
	"TOGGLE_MENU",
	"TOGGLE_ITEM",
	"TOGGLE_BLENDING",
	"TOGGLE_PROFILER",
	"TOGGLE_CONSTRUCT_MODE",
	"TOGGLE_UNIT_DEBUG",
	"TRAIN_OBJECT",
	"ENABLE_BUILDING_PLACEMENT",
	"DISABLE_SET_ABILITY",
	"SET_ABILITY_MOVE",
	"SET_ABILITY_GATHER",
	"SET_ABILITY_GARRISON",
	"TOGGLE_CONSOLE",
	"SPAWN_VILLAGER",
	"KILL_UNIT",
	"BUILD_MENU",
	"BUILD_MENU_MIL",
	"CANCEL",
	"BUILDING_HOUS",
	"BUILDING_MILL",
	"BUILDING_MINE",
	"BUILDING_SMIL",
	"BUILDING_DOCK",
	"BUILDING_FARM",
	"BUILDING_BLAC",
	"BUILDING_MRKT",
	"BUILDING_CRCH",
	"BUILDING_UNIV",
	"BUILDING_RTWC",
	"BUILDING_WNDR",
	"BUILDING_BRKS",
	"BUILDING_ARRG",
	"BUILDING_STBL",
	"BUILDING_SIWS",
	"BUILDING_WCTWX",
	"BUILDING_WALL",
	"BUILDING_WALL2",
	"BUILDING_WCTW",
	"BUILDING_WCTW4",
	"BUILDING_GTCA2",
	"BUILDING_CSTL",
	"BUILDING_TOWN_CENTER",
	"SWITCH_TO_PLAYER_1",
	"SWITCH_TO_PLAYER_2",
	"SWITCH_TO_PLAYER_3",
	"SWITCH_TO_PLAYER_4",
	"SWITCH_TO_PLAYER_5",
	"SWITCH_TO_PLAYER_6",
	"SWITCH_TO_PLAYER_7",
	"SWITCH_TO_PLAYER_8",
	"UP_ARROW",
	"DOWN_ARROW",
	"LEFT_ARROW",
	"RIGHT_ARROW",
	"SELECT",
	"DESELECT",
	"BEGIN_SELECTION",
	"END_SELECTION",
	"FORWARD",
	"BACK",
	"PAINT_TERRAIN",
	"BUILDING_5",
	"BUILDING_6",
	"BUILDING_7",
	"BUILDING_8",
	"BUILD",
	"KEEP_BUILDING",
	"INCREASE_SELECTION",
	"ORDER_SELECT"
};

}


ActionManager::ActionManager(InputManager *input_manager,
                             cvar::CVarManager *cvar_manager)
	:
	input_manager{input_manager},
	cvar_manager{cvar_manager} {

	this->create("UNDEFINED");

	for (auto &type : DEFAULT_ACTIONS) {
		this->create(type);
	}
} // anonymous namespace

bool ActionManager::create(const std::string type) {
	if (this->actions.find(type) != this->actions.end()) {
		// that action is already in the list. fail.
		// TODO: throw an exception instead?
		// for now, just print a warning log message
		log::log(WARN << "can not create action "
			      << util::repr(type) << ": already exists");
		return false;
	}

	action_t action_id = this->next_action_id++;

	this->reverse_map[action_id] = type;
	this->actions[type] = action_id;

	// and the corresponding cvar, which modifies the action bindings
	// TODO: this has nothing to do with the actionmanager!
	//       remove the cvarmanager-access here!
	this->cvar_manager->create(type, std::make_pair(
		// the cvar's getter
		[type, this]() {
			return this->input_manager->get_bind(type);
		},
		// the cvar's setter
		[type, this](const std::string &value) {
			this->input_manager->set_bind(value.c_str(), type);
		}
	));

	return true;
}


action_t ActionManager::get(const std::string &type) {
	auto it = this->actions.find(type);
	if (it != this->actions.end()) {
		return it->second;
	} else {
		return this->actions.at("UNDEFINED");
	}
}


std::string ActionManager::get_name(const action_t action) {
	auto it = this->reverse_map.find(action);
	if (it != this->reverse_map.end()) {
		return it->second;
	} else {
		return "UNDEFINED";
	}
}


bool ActionManager::is(const std::string &type, const action_t action) {
	return this->get(type) == action;
}


}} // openage::input
