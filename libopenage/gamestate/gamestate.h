// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve/events/eventtarget.h"
#include "../curve/map.h"
#include "../util/vector.h"

#include <unordered_map>

namespace openage {
namespace curvepong {

using unit_id_t = uint64_t;
using property_id_t = uint64_t;
using player_id_t = uint64_t;

class NyanObserver : curve::EventTarget {
public:
	NyanObserver(const EventManager &mgr, const nyan::Object &object) :
		EventTarget(mgr) {
		object.onchange([this](const curve::curve_time_t &at){
				this->onchange(at);
			});
	}
};

struct NyanIdentifier {
	nyan::fqon_t fqon;
	nyan::memberid_t member;
};


class Player {
public:
	nyan::Object civilisation;

	std::set<std::weak_ptr> units;
};


class Ability {
public:
	Ability(const nyan::Object &);
	nyan::Object type;
};

class Property : curve::EventTarget {
public:
	Property(const EventManager *mgr) :
		EventTarget(mgr) {};
};

/**
 * Element in the game universe.
 *
 * Everything that is somehow relevant in the game is a Unit.
 */
class Unit : curve::EventTarget {
public:
	Unit (unit_id_t, const nyan::Object &);

	// Has to be kept in sync with the player list
	curve::Discrete<player_id_t> owning_player;

	/** The least common denominator is the position of an object. */
	std::shared_ptr<curve::Continuous<util::Vector<3>>> position;

	/**
	 * List of per-unit variables that are constructed from abilities and
	 * properties from nyan
	 */
	curve::unordered_map<property_id_t, std::shared_ptr<Property>> properties;

	/**
	 * property tracking within the nyan tree
	 *
	 * properties watched by this unit in nyan. This is used to traverse the
	 * on-change events from nyan into the event library.
	 */
	std::unordered_map<NyanIdentifier, std::shared_ptr<NyanObserver>> observed;

	/**
	 * The referenced unit type
	 */
	nyan::Object type;

	/**
	 * The unique identifier of this object.
	 */
	unit_id_t id;
};

class Universe {
public:
	Universe(const nyan::Object &);
	nyan::Object data;

	curve::unordered_map<unit_id_t, Unit> units;

	unit_id_t next_unit_id;
};

class Game {
public:
	Universe universe;
	EventManager evntmgr;
	std::unordered_map<player_id_t, Player> players;
};

}}
