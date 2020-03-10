// Copyright 2014-2020 the openage authors. See copying.md for legal info.

#include <initializer_list>

#include "../engine.h"
#include "../log/log.h"
#include "../gamedata/unit.gen.h"
#include "../terrain/terrain.h"
#include "../terrain/terrain_object.h"
#include "../terrain/terrain_outline.h"
#include "../util/strings.h"
#include "ability.h"
#include "action.h"
#include "producer.h"
#include "unit.h"
#include "unit_texture.h"

/** @file
 * Many values in this file are hardcoded, due to limited understanding of how the original
 * game files work -- as more becomes known these will be removed.
 *
 * It is likely the conversion from gamedata to openage units will be done by the nyan
 * system in future
 */

namespace openage {

std::unordered_set<terrain_t> allowed_terrains(const gamedata::ground_type &restriction) {
	// returns a terrain whitelist for a given restriction
	// you can also define a blacklist for brevity, it will be converted and returned
	std::unordered_set<terrain_t> whitelist;
	std::unordered_set<terrain_t> blacklist;

	// 1, 14, and 15 are water, 2 is shore
	if (restriction == gamedata::ground_type::WATER ||
		restriction == gamedata::ground_type::WATER_0x0D ||
		restriction == gamedata::ground_type::WATER_SHIP_0x03 ||
		restriction == gamedata::ground_type::WATER_SHIP_0x0F) {

		whitelist.insert(1); // water
		whitelist.insert(2); // shore
		whitelist.insert(4); // shallows
		whitelist.insert(14); // medium water
		whitelist.insert(15); // deep water
	}
	else if (restriction == gamedata::ground_type::SOLID) {
		blacklist.insert(1); // water
		blacklist.insert(4); // shallows
		blacklist.insert(14); // medium water
		blacklist.insert(15); // deep water
	}
	else if (restriction == gamedata::ground_type::FOUNDATION ||
		restriction == gamedata::ground_type::NO_ICE_0x08 ||
		restriction == gamedata::ground_type::FOREST) {

		blacklist.insert(1); // water
		blacklist.insert(4); // shallows
		blacklist.insert(14); // medium water
		blacklist.insert(15); // deep water
		blacklist.insert(18); // ice
	}
	else {
		log::log(MSG(warn) << "undefined terrain restriction, assuming solid");
		blacklist.insert(1); // water
		blacklist.insert(4); // shallows
		blacklist.insert(14); // medium water
		blacklist.insert(15); // deep water
	}

	// if we're using a blacklist, fill out a whitelist with everything not on it
	if (blacklist.size() > 0) {
		// Allow all terrains that are not on blacklist
		for (int i = 0; i < 32; ++i) {
			if (blacklist.count(i) == 0) {
				whitelist.insert(i);
			}
		}
	}

	return whitelist;
}

ResourceBundle create_resource_cost(game_resource resource, int amount) {
	ResourceBundle resources = ResourceBundle();
	resources[resource] = amount;
	return resources;
}

ObjectProducer::ObjectProducer(const Player &owner, const GameSpec &spec, const gamedata::unit_object *ud)
	:
	UnitType(owner),
	dataspec(spec),
	unit_data(*ud),
	terrain_outline{nullptr},
	default_tex{spec.get_unit_texture(ud->idle_graphic0)},
	dead_unit_id{ud->dead_unit_id} {

	// copy the class type
	this->unit_class = this->unit_data.unit_class;
	this->icon = this->unit_data.icon_id;

	// for now just look for type names ending with "_D"
	this->decay = unit_data.name.substr(unit_data.name.length() - 2) == "_D";

	// find suitable sounds
	int creation_sound = this->unit_data.train_sound_id;
	int dying_sound = this->unit_data.dying_sound_id;
	if (creation_sound == -1) {
		creation_sound = this->unit_data.damage_sound_id;
	}
	if (creation_sound == -1) {
		creation_sound = this->unit_data.selection_sound_id;
	}
	if (dying_sound == -1) {
		dying_sound = 323; //generic explosion sound
	}
	on_create = spec.get_sound(creation_sound);
	on_destroy = spec.get_sound(dying_sound);

	// convert the float to the discrete foundation size...
	this->foundation_size = {
		static_cast<int>(this->unit_data.radius_x * 2),
		static_cast<int>(this->unit_data.radius_y * 2),
	};

	// shape of the outline
	if (this->unit_data.obstruction_class > 1) {
		this->terrain_outline = radial_outline(this->unit_data.radius_x);
	}
	else {
		this->terrain_outline = square_outline(this->foundation_size);
	}

	// graphic set
	auto standing = spec.get_unit_texture(this->unit_data.idle_graphic0);
	if (!standing) {

		// indicates problems with data converion
		throw Error(MSG(err) << "Unit id " << this->unit_data.id0
			<< " has invalid graphic data, try reconverting the data");
	}
	this->graphics[graphic_type::standing] = standing;
	auto dying_tex = spec.get_unit_texture(this->unit_data.dying_graphic);
	if (dying_tex) {
		this->graphics[graphic_type::dying] = dying_tex;
	}

	// default extra graphics
	this->graphics[graphic_type::attack] = this->graphics[graphic_type::standing];
	this->graphics[graphic_type::work] = this->graphics[graphic_type::standing];

	// pull extra graphics from unit commands
	auto cmds = spec.get_command_data(this->unit_data.id0);
	for (auto cmd : cmds) {

		// same attack / work graphic
		if (cmd->work_sprite_id == -1 && cmd->proceed_sprite_id > 0) {
			auto task = spec.get_unit_texture(cmd->proceed_sprite_id);
			if (task) {
				this->graphics[graphic_type::work] = task;
				this->graphics[graphic_type::attack] = task;
			}
		}

		// separate work and attack graphics
		if (cmd->work_sprite_id > 0 && cmd->proceed_sprite_id > 0 ) {
			auto attack = spec.get_unit_texture(cmd->proceed_sprite_id);
			auto work = spec.get_unit_texture(cmd->work_sprite_id);
			if (attack) {
				this->graphics[graphic_type::attack] = attack;
			}
			if (work) {
				this->graphics[graphic_type::work] = work;
			}
		}

		// villager carrying resources graphics
		if (cmd->carry_sprite_id > 0) {
			auto carry = spec.get_unit_texture(cmd->carry_sprite_id);
			this->graphics[graphic_type::carrying] = carry;
		}
	}

	// TODO get cost, temp fixed cost of 50 food
	this->cost.set(cost_type::constant, create_resource_cost(game_resource::food, 50));

}

ObjectProducer::~ObjectProducer() = default;

int ObjectProducer::id() const {
	return this->unit_data.id0;
}

int ObjectProducer::parent_id() const {
	int uid = this->unit_data.id0;

	// male types
	if (uid == 156 || uid == 120 || uid == 592 || uid == 123 || uid == 579 || uid == 124) {
		return 83;
	}

	// female types
	else if (uid == 222 || uid == 354 || uid == 590 || uid == 218 || uid == 581 || uid == 220) {
		return 293;
	}
	return uid;
}

std::string ObjectProducer::name() const {
	return this->unit_data.name;
}

void ObjectProducer::initialise(Unit *unit, Player &player) {
	ENSURE(this->owner == player, "unit init from a UnitType of a wrong player which breaks tech levels");

	// log attributes
	unit->log(MSG(dbg) << "setting unit type " <<
		this->unit_data.id0 << " " <<
		this->unit_data.name);

	// reset existing attributes
	unit->reset();

	// initialise unit
	unit->unit_type = this;

	// colour
	unit->add_attribute(std::make_shared<Attribute<attr_type::owner>>(player));

	// hitpoints if available
	if (this->unit_data.hit_points > 0) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::hitpoints>>(this->unit_data.hit_points));
		unit->add_attribute(std::make_shared<Attribute<attr_type::damaged>>(this->unit_data.hit_points));
	}

	// collectable resources
	if (this->unit_data.unit_class == gamedata::unit_classes::TREES) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::wood, 125));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::BERRY_BUSH) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::food, 100));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::SEA_FISH) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::food, 200));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::PREY_ANIMAL) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::food, 140));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::HERDABLE) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::food, 100, 0.1));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::GOLD_MINE) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::gold, 800));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::STONE_MINE) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::stone, 350));
	}

	// decaying units have a timed lifespan
	if (decay) {
		unit->push_action(std::make_unique<DecayAction>(unit), true);
	}
	else {
		// if destruction graphic is available
		if (this->dead_unit_id) {
			unit->push_action(
				std::make_unique<DeadAction>(
					unit,
					[this, unit, &player]() {

						// modify unit to have  dead type
						UnitType *t = player.get_type(this->dead_unit_id);
						if (t) {
							t->initialise(unit, player);
						}
					}
				),
				true);
		}
		else if (this->graphics.count(graphic_type::dying) > 0) {
			unit->push_action(std::make_unique<DeadAction>(unit), true);
		}

		// the default action
		unit->push_action(std::make_unique<IdleAction>(unit), true);
	}

	// give required abilitys
	for (auto &a : this->type_abilities) {
		unit->give_ability(a);
	}
}

TerrainObject *ObjectProducer::place(Unit *u, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {

	// create new object with correct base shape
	if (this->unit_data.obstruction_class > 1) {
		u->make_location<RadialObject>(this->unit_data.radius_x, this->terrain_outline);
	}
	else {
		u->make_location<SquareObject>(this->foundation_size, this->terrain_outline);
	}

	// find set of allowed terrains
	std::unordered_set<terrain_t> terrains = allowed_terrains(this->unit_data.terrain_restriction);

	/*
	 * decide what terrain is passable using this lambda
	 * currently unit avoids water and tiles with another unit
	 * this function should be true if pos is a valid position of the object
	 */
	TerrainObject *obj_ptr = u->location.get();
	std::weak_ptr<Terrain> terrain_ptr = terrain;
	u->location->passable = [obj_ptr, terrain_ptr, terrains](const coord::phys3 &pos) -> bool {

		// if location is deleted, then so is this lambda (deleting terrain implies location is deleted)
		// so locking objects here will not return null
		auto terrain = terrain_ptr.lock();

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(obj_ptr->get_range(pos, *terrain))) {
			TileContent *tc = terrain->get_data(check_pos);

			// invalid tile types
			if (!tc || terrains.count(tc->terrain_id) == 0) {
				return false;
			}

			// compare with objects intersecting the units tile
			// ensure no intersections with other objects
			for (auto obj_cmp : tc->obj) {
				if (obj_ptr != obj_cmp &&
				    obj_cmp->check_collisions() &&
				    obj_ptr->intersects(*obj_cmp, pos)) {
					return false;
				}
			}
		}
		return true;
	};

	u->location->draw = [u, obj_ptr](const Engine &e) {
		if (u->selected) {
			obj_ptr->draw_outline(e.coord);
		}
		u->draw(e);
	};

	// try to place the obj, it knows best whether it will fit.
	auto state = this->decay? object_state::placed_no_collision : object_state::placed;
	if (u->location->place(terrain, init_pos, state)) {
		if (this->on_create) {
			this->on_create->play();
		}
		return u->location.get();
	}

	// placing at the given position failed
	u->log(MSG(dbg) << "failed to place object");
	return nullptr;
}

MovableProducer::MovableProducer(const Player &owner, const GameSpec &spec, const gamedata::projectile_unit *um)
	:
	ObjectProducer(owner, spec, um),
	unit_data(*um),
	on_move{spec.get_sound(this->unit_data.command_sound_id)},
	on_attack{spec.get_sound(this->unit_data.command_sound_id)},
	projectile{this->unit_data.missile_unit_id} {

	// extra graphics if available
	// villagers have invalid attack and walk graphics
	// it seems these come from the command data instead
	auto walk = spec.get_unit_texture(this->unit_data.move_graphics);
	if (!walk) {

		// use standing instead
		walk = this->graphics[graphic_type::standing];
	}
	this->graphics[graphic_type::walking] = walk;

	// reuse as carry graphic if not already set
	if (this->graphics.count(graphic_type::carrying) == 0) {
		this->graphics[graphic_type::carrying] = walk;
	}

	auto attack = spec.get_unit_texture(this->unit_data.fight_sprite_id);
	if (attack && attack->is_valid()) {
		this->graphics[graphic_type::attack] = attack;
	}

	// extra abilities
	this->type_abilities.emplace_back(std::make_shared<MoveAbility>(this->on_move));
	this->type_abilities.emplace_back(std::make_shared<AttackAbility>(this->on_attack));
}

MovableProducer::~MovableProducer() = default;

void MovableProducer::initialise(Unit *unit, Player &player) {

	/*
	 * call base function
	 */
	ObjectProducer::initialise(unit, player);

	/*
	 * basic attributes
	 */
	if (!unit->has_attribute(attr_type::direction)) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::direction>>(coord::phys3_delta{ 1, 0, 0 }));
	}

	/*
	 * distance per millisecond -- consider original game speed
	 * where 1.5 in game seconds pass in 1 real second
	 */
	coord::phys_t sp = this->unit_data.speed / 666;
	unit->add_attribute(std::make_shared<Attribute<attr_type::speed>>(sp));

	// projectile of melee attacks
	UnitType *proj_type = this->owner.get_type(this->projectile);
	if (this->unit_data.missile_unit_id > 0 && proj_type) {

		// calculate requirements for ranged attacks
		coord::phys_t range_phys = this->unit_data.weapon_range_max;
		unit->add_attribute(std::make_shared<Attribute<attr_type::attack>>(proj_type, range_phys, 48000, 1));
	}
	else {
		unit->add_attribute(std::make_shared<Attribute<attr_type::attack>>(nullptr, 0, 0, 1));
	}
	unit->add_attribute(std::make_shared<Attribute<attr_type::formation>>());
}

TerrainObject *MovableProducer::place(Unit *unit, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {
	return ObjectProducer::place(unit, terrain, init_pos);
}

LivingProducer::LivingProducer(const Player &owner, const GameSpec &spec, const gamedata::living_unit *ud)
	:
	MovableProducer(owner, spec, ud),
	unit_data(*ud) {

	// extra abilities
	this->type_abilities.emplace_back(std::make_shared<GarrisonAbility>(this->on_move));
}

LivingProducer::~LivingProducer() = default;

void LivingProducer::initialise(Unit *unit, Player &player) {

	/*
	 * call base function
	 */
	MovableProducer::initialise(unit, player);

	// population of 1 for all movable units
	if (this->unit_data.unit_class != gamedata::unit_classes::HERDABLE) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::population>>(1, 0));
	}

	// add worker attributes
	if (this->unit_data.unit_class == gamedata::unit_classes::CIVILIAN) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::worker>>());
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>());
		unit->add_attribute(std::make_shared<Attribute<attr_type::multitype>>());

		// add graphic ids for resource actions
		auto &worker_attr = unit->get_attribute<attr_type::worker>();
		worker_attr.capacity = 10.0;
		worker_attr.gather_rate[game_resource::wood] = 0.002;
		worker_attr.gather_rate[game_resource::food] = 0.002;
		worker_attr.gather_rate[game_resource::gold] = 0.002;
		worker_attr.gather_rate[game_resource::stone] = 0.002;

		auto &multitype_attr = unit->get_attribute<attr_type::multitype>();
		// currently not sure where the game data keeps these values
		// todo PREY_ANIMAL SEA_FISH
		if (this->parent_id() == 83) {

			// male graphics
			multitype_attr.types[gamedata::unit_classes::CIVILIAN] = this->parent_type(); // get default villager
			multitype_attr.types[gamedata::unit_classes::BUILDING] = this->owner.get_type(156); // builder 118
			multitype_attr.types[gamedata::unit_classes::BERRY_BUSH] = this->owner.get_type(120); // forager
			multitype_attr.types[gamedata::unit_classes::HERDABLE] = this->owner.get_type(592); // sheperd
			multitype_attr.types[gamedata::unit_classes::TREES] = this->owner.get_type(123); // woodcutter
			multitype_attr.types[gamedata::unit_classes::GOLD_MINE] = this->owner.get_type(579); // gold miner
			multitype_attr.types[gamedata::unit_classes::STONE_MINE] = this->owner.get_type(124); // stone miner

		}
		else {

			// female graphics
			multitype_attr.types[gamedata::unit_classes::CIVILIAN] = this->parent_type(); // get default villager
			multitype_attr.types[gamedata::unit_classes::BUILDING] = this->owner.get_type(222); // builder 212
			multitype_attr.types[gamedata::unit_classes::BERRY_BUSH] = this->owner.get_type(354); // forager
			multitype_attr.types[gamedata::unit_classes::HERDABLE] = this->owner.get_type(590); // sheperd
			multitype_attr.types[gamedata::unit_classes::TREES] = this->owner.get_type(218); // woodcutter
			multitype_attr.types[gamedata::unit_classes::GOLD_MINE] = this->owner.get_type(581); // gold miner
			multitype_attr.types[gamedata::unit_classes::STONE_MINE] = this->owner.get_type(220); // stone miner
		}
		unit->give_ability(std::make_shared<GatherAbility>(this->on_attack));
		unit->give_ability(std::make_shared<BuildAbility>(this->on_attack));
		unit->give_ability(std::make_shared<RepairAbility>(this->on_attack));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::FISHING_BOAT) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::worker>>());
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>());

		// add fishing abilites
		auto &worker_attr = unit->get_attribute<attr_type::worker>();
		worker_attr.capacity = 15.0;
		worker_attr.gather_rate[game_resource::food] = 0.002;

		unit->give_ability(std::make_shared<GatherAbility>(this->on_attack));
	}
}

TerrainObject *LivingProducer::place(Unit *unit, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {
	return MovableProducer::place(unit, terrain, init_pos);
}

BuildingProducer::BuildingProducer(const Player &owner, const GameSpec &spec, const gamedata::building_unit *ud)
	:
	UnitType(owner),
	unit_data{*ud},
	texture{spec.get_unit_texture(ud->idle_graphic0)},
	destroyed{spec.get_unit_texture(ud->dying_graphic)},
	projectile{this->unit_data.missile_unit_id},
	foundation_terrain{ud->foundation_terrain_id},
	enable_collisions{this->unit_data.id0 != 109} { // 109 = town center

	// copy the class type
	this->unit_class = this->unit_data.unit_class;
	this->icon = this->unit_data.icon_id;

	// find suitable sounds
	int creation_sound = this->unit_data.train_sound_id;
	int dying_sound = this->unit_data.dying_sound_id;
	if (creation_sound == -1) {
		creation_sound = this->unit_data.damage_sound_id;
	}
	if (creation_sound == -1) {
		creation_sound = this->unit_data.selection_sound_id;
	}
	if (dying_sound == -1) {
		dying_sound = 323; //generic explosion sound
	}
	on_create = spec.get_sound(creation_sound);
	on_destroy = spec.get_sound(dying_sound);

	// convert the float to the discrete foundation size...
	this->foundation_size = {
		static_cast<int>(this->unit_data.radius_x * 2),
		static_cast<int>(this->unit_data.radius_y * 2),
	};

	// graphic set
	this->graphics[graphic_type::construct] = spec.get_unit_texture(ud->construction_graphic_id);
	this->graphics[graphic_type::standing] = spec.get_unit_texture(ud->idle_graphic0);
	this->graphics[graphic_type::attack] = spec.get_unit_texture(ud->idle_graphic0);
	auto dying_tex = spec.get_unit_texture(ud->dying_graphic);
	if (dying_tex) {
		this->graphics[graphic_type::dying] = dying_tex;
	}

	this->terrain_outline = square_outline(this->foundation_size);

	// TODO get cost, temp fixed cost of 100 wood
	this->cost.set(cost_type::constant, create_resource_cost(game_resource::wood, 100));
}

BuildingProducer::~BuildingProducer() = default;

int BuildingProducer::id() const {
	return this->unit_data.id0;
}

int BuildingProducer::parent_id() const {
	return this->unit_data.id0;
}

std::string BuildingProducer::name() const {
	return this->unit_data.name;
}

void BuildingProducer::initialise(Unit *unit, Player &player) {
	ENSURE(this->owner == player, "unit init from a UnitType of a wrong player which breaks tech levels");

	// log type
	unit->log(MSG(dbg) << "setting unit type " <<
		this->unit_data.id0 << " " <<
		this->unit_data.name);

	// initialize graphic set
	unit->unit_type = this;

	auto player_attr = std::make_shared<Attribute<attr_type::owner>>(player);
	unit->add_attribute(player_attr);

	// building specific attribute
	auto build_attr = std::make_shared<Attribute<attr_type::building>>(
		this->foundation_terrain,
		this->owner.get_type(293), // fem_villager, male is 83
		unit->location->pos.draw
	);
	build_attr->completion_state = this->enable_collisions ? object_state::placed : object_state::placed_no_collision;
	unit->add_attribute(build_attr);

	// garrison and hp for all buildings
	unit->add_attribute(std::make_shared<Attribute<attr_type::garrison>>());
	unit->add_attribute(std::make_shared<Attribute<attr_type::hitpoints>>(this->unit_data.hit_points));
	unit->add_attribute(std::make_shared<Attribute<attr_type::damaged>>(this->unit_data.hit_points));

	// population
	if (this->id() == 109 || this->id() == 70) { // Town center, House
		unit->add_attribute(std::make_shared<Attribute<attr_type::population>>(0, 5));
	}
	else if (this->id() == 82) { // Castle
		unit->add_attribute(std::make_shared<Attribute<attr_type::population>>(0, 20));
	}

	// limits
	if (this->id() == 109 || this->id() == 276) { // Town center, Wonder
		this->have_limit = 2; // TODO change to 1, 2 is for testing
	}

	bool has_destruct_graphic = this->destroyed != nullptr;
	unit->push_action(std::make_unique<FoundationAction>(unit, has_destruct_graphic), true);

	UnitType *proj_type = this->owner.get_type(this->projectile);
	if (this->unit_data.missile_unit_id > 0 && proj_type) {
		coord::phys_t range_phys = this->unit_data.weapon_range_max;
		unit->add_attribute(std::make_shared<Attribute<attr_type::attack>>(proj_type, range_phys, 350000, 1));
		// formation is used only for the attack_stance
		unit->add_attribute(std::make_shared<Attribute<attr_type::formation>>(attack_stance::aggressive));
		unit->give_ability(std::make_shared<AttackAbility>());
	}

	// dropsite attribute
	std::vector<game_resource> accepted_resources = this->get_accepted_resources();
	if (accepted_resources.size() != 0) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::dropsite>>(accepted_resources));
	}

	// building can train new units and ungarrison
	unit->give_ability(std::make_shared<SetPointAbility>());
	unit->give_ability(std::make_shared<TrainAbility>());
	unit->give_ability(std::make_shared<UngarrisonAbility>());
}

std::vector<game_resource> BuildingProducer::get_accepted_resources() {
	//TODO use a more general approach instead of hard coded ids

	auto id_in = [=](std::initializer_list<int> ids) {
		return std::any_of(ids.begin(), ids.end(), [=](int n) { return n == this->id(); });
	};

	if (this->id() == 109) { // Town center
		return std::vector<game_resource>{
			game_resource::wood,
			game_resource::food,
			game_resource::gold,
			game_resource::stone
		};
	} else if (id_in({584, 585, 586, 587})) { // Mine
		return std::vector<game_resource>{
			game_resource::gold,
			game_resource::stone
		};
	} else if (id_in({68, 129, 130, 131})) { // Mill
		return std::vector<game_resource>{
			game_resource::food
		};
	} else if (id_in({562, 563, 564, 565})) { // Lumberjack camp
		return std::vector<game_resource>{
			game_resource::wood
		};
	}

	return std::vector<game_resource>();
}

TerrainObject *BuildingProducer::place(Unit *u, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {

	// buildings have a square base
	u->make_location<SquareObject>(this->foundation_size, this->terrain_outline);

	/*
	 * decide what terrain is passable using this lambda
	 * currently unit avoids water and tiles with another unit
	 * this function should be true if pos is a valid position of the object
	 */
	TerrainObject *obj_ptr = u->location.get();
	std::weak_ptr<Terrain> terrain_ptr = terrain;

	// find set of allowed terrains
	std::unordered_set<terrain_t> terrains = allowed_terrains(this->unit_data.terrain_restriction);

	u->location->passable = [obj_ptr, terrain_ptr, terrains](const coord::phys3 &pos) -> bool {
		auto terrain = terrain_ptr.lock();

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(obj_ptr->get_range(pos, *terrain))) {
			TileContent *tc = terrain->get_data(check_pos);

			// check if terrains are suitable and free of content
			if (!tc || !terrains.count(tc->terrain_id) || tc->obj.size()) {
				return false;
			}
		}

		return true;
	};

	// drawing function
	bool draw_outline = this->enable_collisions;
	u->location->draw = [u, obj_ptr, draw_outline](const Engine &e) {
		if (u->selected && draw_outline) {
			obj_ptr->draw_outline(e.coord);
		}
		u->draw(e);
	};

	// try to place the obj, it knows best whether it will fit.
	auto state = object_state::floating;
	if (!u->location->place(terrain, init_pos, state)) {
		return nullptr;
	}

	// annex objects
	for (unsigned i = 0; i < 4; ++i) {
		const gamedata::building_annex &annex = this->unit_data.building_annex.data[i];
		if (annex.unit_id > 0) {

			// make objects for annex
			coord::phys3 a_pos = u->location->pos.draw;
			a_pos.ne += annex.misplaced0;
			a_pos.se += annex.misplaced1;
			this->make_annex(*u, terrain, annex.unit_id, a_pos, i == 0);
		}
	}

	// TODO: play sound once built
	if (this->on_create) {
		this->on_create->play();
	}
	return u->location.get();
}

TerrainObject *BuildingProducer::make_annex(Unit &u, std::shared_ptr<Terrain> t,
                                            int annex_id, coord::phys3 annex_pos, bool c) const {

	// for use in lambda drawing functions
	auto annex_type = this->owner.get_type(annex_id);
	if (!annex_type) {
		u.log(MSG(warn) << "Invalid annex type id " << annex_id);
		return nullptr;
	}

	// foundation size
	coord::tile_delta annex_foundation = annex_type->foundation_size;

	// producers place by the nw tile
	coord::phys3 start_tile = annex_pos;
	start_tile.ne -= annex_foundation.ne / 2.0;
	start_tile.se -= annex_foundation.se / 2.0;

	// create and place on terrain
	TerrainObject *annex_loc = u.location->make_annex<SquareObject>(annex_foundation);
	object_state state = c? object_state::placed : object_state::placed_no_collision;
	annex_loc->place(t, start_tile, state);

	// create special drawing functions for annexes,
	annex_loc->draw = [annex_loc, annex_type, &u, c](const Engine &e) {

		// hack which draws the outline in the right order
		// removable once rendering system is improved
		if (c && u.selected) {
			annex_loc->get_parent()->draw_outline(e.coord);
		}

		// only draw if building is completed
		if (u.has_attribute(attr_type::building) &&
		    u.get_attribute<attr_type::building>().completed >= 1.0f) {
			u.draw(annex_loc, annex_type->graphics, e);
		}
	};
	return annex_loc;
}

ProjectileProducer::ProjectileProducer(const Player &owner, const GameSpec &spec, const gamedata::missile_unit *pd)
	:
	UnitType(owner),
	unit_data{*pd},
	tex{spec.get_unit_texture(this->unit_data.idle_graphic0)},
	sh{spec.get_unit_texture(3379)}, // 3379 = general arrow shadow
	destroyed{spec.get_unit_texture(this->unit_data.dying_graphic)} {

	// copy the class type
	this->unit_class = this->unit_data.unit_class;

	// graphic set
	this->graphics[graphic_type::standing] = this->tex;
	this->graphics[graphic_type::shadow] = this->sh;
	if (destroyed) {
		this->graphics[graphic_type::dying] = destroyed;
	}

	// outline
	terrain_outline = radial_outline(pd->radius_y);
}

ProjectileProducer::~ProjectileProducer() = default;

int ProjectileProducer::id() const {
	return this->unit_data.id0;
}

int ProjectileProducer::parent_id() const {
	return this->unit_data.id0;
}

std::string ProjectileProducer::name() const {
	return this->unit_data.name;
}

void ProjectileProducer::initialise(Unit *unit, Player &player) {
	ENSURE(this->owner == player, "unit init from a UnitType of a wrong player which breaks tech levels");

	// initialize graphic set
	unit->unit_type = this;

	auto player_attr = std::make_shared<Attribute<attr_type::owner>>(player);
	unit->add_attribute(player_attr);

	// projectile speed
	coord::phys_t sp = this->unit_data.speed / 666;
	unit->add_attribute(std::make_shared<Attribute<attr_type::speed>>(sp));
	unit->add_attribute(std::make_shared<Attribute<attr_type::projectile>>(this->unit_data.projectile_arc));
	unit->add_attribute(std::make_shared<Attribute<attr_type::direction>>(coord::phys3_delta{ 1, 0, 0 }));

	// if destruction graphic is available
	if (this->destroyed) {
		unit->push_action(std::make_unique<DeadAction>(unit), true);
	}
}

TerrainObject *ProjectileProducer::place(Unit *u, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {
	/*
	 * radial base shape without collision checking
	 */
	u->make_location<RadialObject>(this->unit_data.radius_y, this->terrain_outline);

	TerrainObject *obj_ptr = u->location.get();
	std::weak_ptr<Terrain> terrain_ptr = terrain;
	u->location->passable = [obj_ptr, u, terrain_ptr](const coord::phys3 &pos) -> bool {
		if (pos.up > 64000) {
			return true;
		}

		// avoid intersections with launcher
		Unit *launcher = nullptr;
		auto terrain = terrain_ptr.lock();
		if (u->has_attribute(attr_type::projectile)) {
			auto &pr_attr = u->get_attribute<attr_type::projectile>();
			if (pr_attr.launched && pr_attr.launcher.is_valid()) {
				launcher = pr_attr.launcher.get();
			}
			else {
				return true;
			}
		}
		else {
			return true;
		}

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(obj_ptr->get_range(pos, *terrain))) {
			TileContent *tc = terrain->get_data(check_pos);
			if (!tc) return false;

			// ensure no intersections with other objects
			for (auto obj_cmp : tc->obj) {
				if (obj_ptr != obj_cmp &&
				    &obj_cmp->unit != launcher &&
				    obj_cmp->check_collisions() &&
				    obj_ptr->intersects(*obj_cmp, pos)) {
					return false;
				}
			}
		}
		return true;
	};

	u->location->draw = [u](const Engine &e) {
		u->draw(e);
	};

	// try to place the obj, it knows best whether it will fit.
	if (u->location->place(terrain, init_pos, object_state::placed_no_collision)) {
		return u->location.get();
	}
	return nullptr;
}

} // namespace openage
