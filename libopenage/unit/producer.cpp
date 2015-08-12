// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "../gamedata/unit.gen.h"
#include "../terrain/terrain.h"
#include "../terrain/terrain_object.h"
#include "../terrain/terrain_outline.h"
#include "../util/strings.h"
#include "../game_spec.h"
#include "../log/log.h"
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
	std::unordered_set<terrain_t> result;

	// 1, 14, and 15 are water, 2 is shore
	if (restriction == gamedata::ground_type::WATER ||
	    restriction == gamedata::ground_type::WATER_0x0D ||
	    restriction == gamedata::ground_type::WATER_SHIP_0x03 ||
	    restriction == gamedata::ground_type::WATER_SHIP_0x0F) {
		result.insert(1);
		result.insert(2);
		result.insert(14);
		result.insert(15);
	}
	else {
		for (int i = 0; i < 32; ++i) {
			if (i != 1 && i != 14 && i != 15) {
				result.insert(i);
			}
		}
	}

	return result;
}

ObjectProducer::ObjectProducer(GameSpec &spec, const gamedata::unit_object *ud)
	:
	dataspec(spec),
	unit_data(*ud),
	terrain_outline{nullptr},
	default_tex{spec.get_unit_texture(ud->graphic_standing0)},
	dead_unit_producer{spec.get_type(ud->dead_unit_id)} {

	// for now just look for type names ending with "_D"
	this->decay = unit_data.name.substr(unit_data.name.length() - 2) == "_D";

	// find suitable sounds
	int creation_sound = this->unit_data.sound_creation0;
	int dying_sound = this->unit_data.sound_dying;
	if (creation_sound == -1) {
		creation_sound = this->unit_data.sound_creation1;
	}
	if (creation_sound == -1) {
		creation_sound = this->unit_data.sound_selection;
	}
	if (dying_sound == -1) {
		dying_sound = 323; //generic explosion sound
	}
	on_create = spec.get_sound(creation_sound);
	on_destroy = spec.get_sound(dying_sound);

	// convert the float to the discrete foundation size...
	this->foundation_size = {
		static_cast<int>(this->unit_data.radius_size0 * 2),
		static_cast<int>(this->unit_data.radius_size1 * 2),
	};

	// shape of the outline
	if (this->unit_data.selection_shape > 1) {
		this->terrain_outline = radial_outline(this->unit_data.radius_size0);
	}
	else {
		this->terrain_outline = square_outline(this->foundation_size);
	}

	// graphic set
	auto standing = spec.get_unit_texture(this->unit_data.graphic_standing0);
	if (!standing) {

		// indicates problems with data converion
		throw Error(MSG(err) << "Unit id " << this->unit_data.id0
			<< " has invalid graphic data, try reconverting the data");
	}
	this->graphics[graphic_type::standing] = standing;
	auto dying_tex = spec.get_unit_texture(this->unit_data.graphic_dying0);
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
		if (cmd->action_graphic_id == -1 && cmd->proceed_graphic_id > 0) {
			auto task = spec.get_unit_texture(cmd->proceed_graphic_id);
			if (task) {
				this->graphics[graphic_type::work] = task;
				this->graphics[graphic_type::attack] = task;
			}
		}

		// seperate work and attack graphics
		if (cmd->action_graphic_id > 0 && cmd->proceed_graphic_id > 0 ) {
			auto attack = spec.get_unit_texture(cmd->proceed_graphic_id);
			auto work = spec.get_unit_texture(cmd->action_graphic_id);
			if (attack) {
				this->graphics[graphic_type::attack] = attack;
			}
			if (work) {
				this->graphics[graphic_type::work] = work;
			}
		}

		// villager carrying resources graphics
		if (cmd->carrying_graphic_id > 0) {
			auto carry = spec.get_unit_texture(cmd->carrying_graphic_id);
			this->graphics[graphic_type::carrying] = carry;
			if (carry) {

			}
		}
	}
}

ObjectProducer::~ObjectProducer() {}

int ObjectProducer::id() const {
	return this->unit_data.id0;
}

std::string ObjectProducer::name() const {
	return this->unit_data.name;
}

void ObjectProducer::initialise(Unit *unit, Player &player) {

	// log attributes
	unit->log(MSG(dbg) << "setting unit type " <<
		this->unit_data.id0 << " " <<
		this->unit_data.name);

	// reset existing attributes
	unit->reset();

	// initialise unit
	unit->unit_type = this;
	unit->unit_class = this->unit_data.unit_class;
	unit->graphics = &this->graphics;

	// colour
	unit->add_attribute(std::make_shared<Attribute<attr_type::owner>>(player));

	// hitpoints if available
	if (this->unit_data.hit_points > 0) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::hitpoints>>(this->unit_data.hit_points));
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
	else if (this->unit_data.unit_class == gamedata::unit_classes::SHEEP) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::resource>>(game_resource::food, 100));
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
		if (this->dead_unit_producer) {
			unit->push_action(
				std::make_unique<DeadAction>(
					unit,
					[this, unit, &player]() {

						// modify unit to have  dead type
						this->dead_unit_producer->initialise(unit, player);
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
	if (this->unit_data.selection_shape > 1) {
		u->make_location<RadialObject>(this->unit_data.radius_size0, this->terrain_outline);
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
		for (coord::tile check_pos : tile_list(obj_ptr->get_range(pos))) {
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

	u->location->draw = [u, obj_ptr]() {
		if (u->selected) {
			obj_ptr->draw_outline();
		}
		u->draw();
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

MovableProducer::MovableProducer(GameSpec &spec, const gamedata::unit_movable *um)
	:
	ObjectProducer(spec, um),
	unit_data(*um),
	on_move{spec.get_sound(this->unit_data.move_sound)},
	on_attack{spec.get_sound(this->unit_data.move_sound)},
	projectile{spec.get_type(this->unit_data.projectile_unit_id)} {

	// extra graphics if available
	// villagers have invalid attack and walk graphics
	// it seems these come from the command data instead
	auto walk = spec.get_unit_texture(this->unit_data.walking_graphics0);
	if (!walk) {

		// use standing instead
		walk = this->graphics[graphic_type::standing];
	}
	this->graphics[graphic_type::walking] = walk;

	// reuse as carry graphic if not already set
	if (this->graphics.count(graphic_type::carrying) == 0) {
		this->graphics[graphic_type::carrying] = walk;
	}

	auto attack = spec.get_unit_texture(this->unit_data.attack_graphic);
	if (attack && attack->is_valid()) {
		this->graphics[graphic_type::attack] = attack;
	}

	// extra abilities
	this->type_abilities.emplace_back(std::make_shared<MoveAbility>(this->on_move));
	this->type_abilities.emplace_back(std::make_shared<AttackAbility>(this->on_attack));
}

MovableProducer::~MovableProducer() {}

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
	coord::phys_t sp = this->unit_data.speed * coord::settings::phys_per_tile / 666;
	unit->add_attribute(std::make_shared<Attribute<attr_type::speed>>(sp));

	// projectile of melee attacks
	if (this->unit_data.projectile_unit_id > 0 && this->projectile) {

		// calculate requirements for ranged attacks
		coord::phys_t range_phys = coord::settings::phys_per_tile * this->unit_data.max_range;
		unit->add_attribute(std::make_shared<Attribute<attr_type::attack>>(this->projectile, range_phys, 48000, 1, this->graphics));
	}
	else {
		unit->add_attribute(std::make_shared<Attribute<attr_type::attack>>(nullptr, 0, 0, 1, this->graphics));
	}
}

TerrainObject *MovableProducer::place(Unit *unit, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {
	return ObjectProducer::place(unit, terrain, init_pos);
}

LivingProducer::LivingProducer(GameSpec &spec, const gamedata::unit_living *ud)
	:
	MovableProducer(spec, ud),
	unit_data(*ud) {

	// extra abilities
	this->type_abilities.emplace_back(std::make_shared<GarrisonAbility>(this->on_move));
}

LivingProducer::~LivingProducer() {}

void LivingProducer::initialise(Unit *unit, Player &player) {

	/*
	 * call base function
	 */
	MovableProducer::initialise(unit, player);

	// add worker attributes
	if (this->unit_data.unit_class == gamedata::unit_classes::CIVILIAN) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::gatherer>>());

		// add graphic ids for resource actions
		auto &gather_attr = unit->get_attribute<attr_type::gatherer>();
		gather_attr.current_type = game_resource::wood;
		gather_attr.capacity = 10.0f;
		gather_attr.gather_rate = 0.002f;

		// currently not sure where the game data keeps these values
		// todo PREY_ANIMAL SEA_FISH
		if (this->unit_data.id0 == 83) {

			// male graphics
			gather_attr.graphics[gamedata::unit_classes::BUILDING] = this->dataspec.get_type(156); // builder 118
			gather_attr.graphics[gamedata::unit_classes::BERRY_BUSH] = this->dataspec.get_type(120); // forager
			gather_attr.graphics[gamedata::unit_classes::SHEEP] = this->dataspec.get_type(592); // sheperd
			gather_attr.graphics[gamedata::unit_classes::TREES] = this->dataspec.get_type(123); // woodcutter
			gather_attr.graphics[gamedata::unit_classes::GOLD_MINE] = this->dataspec.get_type(579); // gold miner
			gather_attr.graphics[gamedata::unit_classes::STONE_MINE] = this->dataspec.get_type(124); // stone miner

		}
		else {

			// female graphics
			gather_attr.graphics[gamedata::unit_classes::BUILDING] = this->dataspec.get_type(222); // builder 212
			gather_attr.graphics[gamedata::unit_classes::BERRY_BUSH] = this->dataspec.get_type(354); // forager
			gather_attr.graphics[gamedata::unit_classes::SHEEP] = this->dataspec.get_type(590); // sheperd
			gather_attr.graphics[gamedata::unit_classes::TREES] = this->dataspec.get_type(218); // woodcutter
			gather_attr.graphics[gamedata::unit_classes::GOLD_MINE] = this->dataspec.get_type(581); // gold miner
			gather_attr.graphics[gamedata::unit_classes::STONE_MINE] = this->dataspec.get_type(220); // stone miner
		}
		unit->give_ability(std::make_shared<GatherAbility>(this->on_attack));
		unit->give_ability(std::make_shared<BuildAbility>(this->on_attack));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::FISHING_BOAT) {
		unit->add_attribute(std::make_shared<Attribute<attr_type::gatherer>>());

		// add fishing abilites
		auto &gather_attr = unit->get_attribute<attr_type::gatherer>();
		gather_attr.current_type = game_resource::food;
		gather_attr.capacity = 15.0f;
		gather_attr.gather_rate = 0.002f;
		gather_attr.graphics[gamedata::unit_classes::SEA_FISH] = this;

		unit->give_ability(std::make_shared<GatherAbility>(this->on_attack));
	}
}

TerrainObject *LivingProducer::place(Unit *unit, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {
	return MovableProducer::place(unit, terrain, init_pos);
}

BuildingProducer::BuildingProducer(GameSpec &spec, const gamedata::unit_building *ud)
	:
	dataspec(spec),
	unit_data{*ud},
	texture{spec.get_unit_texture(ud->graphic_standing0)},
	destroyed{spec.get_unit_texture(ud->graphic_dying0)},
	trainable1{spec.get_type(83)}, // 83 = m villager
	trainable2{spec.get_type(293)}, // 293 = f villager
	projectile{spec.get_type(this->unit_data.projectile_unit_id)},
	foundation_terrain{ud->terrain_id},
	enable_collisions{this->unit_data.id0 != 109} { // 109 = town center

	// find suitable sounds
	int creation_sound = this->unit_data.sound_creation0;
	int dying_sound = this->unit_data.sound_dying;
	if (creation_sound == -1) {
		creation_sound = this->unit_data.sound_creation1;
	}
	if (creation_sound == -1) {
		creation_sound = this->unit_data.sound_selection;
	}
	if (dying_sound == -1) {
		dying_sound = 323; //generic explosion sound
	}
	on_create = spec.get_sound(creation_sound);
	on_destroy = spec.get_sound(dying_sound);

	// convert the float to the discrete foundation size...
	this->foundation_size = {
		static_cast<int>(this->unit_data.radius_size0 * 2),
		static_cast<int>(this->unit_data.radius_size1 * 2),
	};

	// graphic set
	this->graphics[graphic_type::construct] = spec.get_unit_texture(ud->construction_graphic_id);
	this->graphics[graphic_type::standing] = spec.get_unit_texture(ud->graphic_standing0);
	this->graphics[graphic_type::attack] = spec.get_unit_texture(ud->graphic_standing0);
	auto dying_tex = spec.get_unit_texture(ud->graphic_dying0);
	if (dying_tex) {
		this->graphics[graphic_type::dying] = dying_tex;
	}

	this->terrain_outline = square_outline(this->foundation_size);
}

BuildingProducer::~BuildingProducer() {}

int BuildingProducer::id() const {
	return this->unit_data.id0;
}

std::string BuildingProducer::name() const {
	return this->unit_data.name;
}

void BuildingProducer::initialise(Unit *unit, Player &player) {

	// log type
	unit->log(MSG(dbg) << "setting unit type " <<
		this->unit_data.id0 << " " <<
		this->unit_data.name);

	// initialize graphic set
	unit->unit_type = this;
	unit->unit_class = this->unit_data.unit_class;
	unit->graphics = &this->graphics;

	auto player_attr = std::make_shared<Attribute<attr_type::owner>>(player);
	unit->add_attribute(player_attr);

	// building specific attribute
	auto build_attr = std::make_shared<Attribute<attr_type::building>>();
	build_attr->foundation_terrain = this->foundation_terrain;
	build_attr->pp = trainable2;
	build_attr->gather_point = unit->location->pos.draw;
	build_attr->completion_state = this->enable_collisions? object_state::placed : object_state::placed_no_collision;
	unit->add_attribute(build_attr);

	// garrison and hp for all buildings
	unit->add_attribute(std::make_shared<Attribute<attr_type::garrison>>());
	unit->add_attribute(std::make_shared<Attribute<attr_type::hitpoints>>(this->unit_data.hit_points));

	bool has_destruct_graphic = this->destroyed != nullptr;
	unit->push_action(std::make_unique<FoundationAction>(unit, has_destruct_graphic), true);

	if (this->unit_data.projectile_unit_id > 0 && this->projectile) {
		coord::phys_t range_phys = coord::settings::phys_per_tile * this->unit_data.max_range;
		unit->add_attribute(std::make_shared<Attribute<attr_type::attack>>(this->projectile, range_phys, 350000, 1, this->graphics));
		unit->give_ability(std::make_shared<AttackAbility>());
	}

	// building can train new units and ungarrison
	unit->give_ability(std::make_shared<SetPointAbility>());
	unit->give_ability(std::make_shared<TrainAbility>());
	unit->give_ability(std::make_shared<UngarrisonAbility>());
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
	u->location->passable = [obj_ptr, terrain_ptr](const coord::phys3 &pos) -> bool {
		auto terrain = terrain_ptr.lock();

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(obj_ptr->get_range(pos))) {
			TileContent *tc = terrain->get_data(check_pos);
			if (!tc) {
				return false;
			}
			for (auto tobj : tc->obj) {
				if (tobj->check_collisions()) return false;
			}
		}
		return true;
	};

	// drawing function
	bool draw_outline = this->enable_collisions;
	u->location->draw = [u, obj_ptr, draw_outline]() {
		if (u->selected && draw_outline) {
			obj_ptr->draw_outline();
		}
		u->draw();
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
			a_pos.ne += annex.misplaced0 * coord::settings::phys_per_tile;
			a_pos.se += annex.misplaced1 * coord::settings::phys_per_tile;
			this->make_annex(*u, terrain, annex.unit_id, a_pos, i == 0);
		}
	}

	// TODO: play sound once built
	if (this->on_create) {
		this->on_create->play();
	}
	return u->location.get();
}

TerrainObject *BuildingProducer::make_annex(Unit &u, std::shared_ptr<Terrain> t, int annex_id, coord::phys3 annex_pos, bool c) const {

	// find annex foundation size
	auto b = this->dataspec.get_building_data(annex_id);
	if (b) {
		u.log(MSG(dbg) << "Adding annex " << annex_id);
	}
	else {
		u.log(MSG(warn) << "Invalid annex data id " << annex_id);
		return nullptr;
	}

	// for use in lambda drawing functions
	auto annex_type = this->dataspec.get_type(annex_id);
	if (!annex_type) {
		u.log(MSG(warn) << "Invalid annex type id " << annex_id);
		return nullptr;
	}

	// foundation size
	coord::tile_delta annex_foundation = {
		static_cast<int>(b->radius_size0 * 2),
		static_cast<int>(b->radius_size1 * 2),
	};

	// producers place by the nw tile
	coord::phys3 start_tile = annex_pos;
	start_tile.ne -= b->radius_size0 * coord::settings::phys_per_tile;
	start_tile.se -= b->radius_size1 * coord::settings::phys_per_tile;

	// create and place on terrain
	TerrainObject *annex_loc = u.location->make_annex<SquareObject>(annex_foundation);
	object_state state = c? object_state::placed : object_state::placed_no_collision;
	annex_loc->place(t, start_tile, state);

	// create special drawing functions for annexes,
	annex_loc->draw = [annex_loc, annex_type, &u, c]() {

		// hack which draws the outline in the right order
		// removable once rendering system is improved
		if (c && u.selected) {
			annex_loc->get_parent()->draw_outline();
		}

		// only draw if building is completed
		if (u.has_attribute(attr_type::building) &&
		    u.get_attribute<attr_type::building>().completed >= 1.0f) {
			u.draw(annex_loc, &annex_type->graphics);
		}
	};
	return annex_loc;
}

ProjectileProducer::ProjectileProducer(GameSpec &spec, const gamedata::unit_projectile *pd)
	:
	unit_data{*pd},
	tex{spec.get_unit_texture(this->unit_data.graphic_standing0)},
	sh{spec.get_unit_texture(3379)}, // 3379 = general arrow shadow
	destroyed{spec.get_unit_texture(this->unit_data.graphic_dying0)} {

	// graphic set
	this->graphics[graphic_type::standing] = this->tex;
	this->graphics[graphic_type::shadow] = this->sh;
	if (destroyed) {
		this->graphics[graphic_type::dying] = destroyed;
	}

	// outline
	terrain_outline = radial_outline(pd->radius_size1);
}

ProjectileProducer::~ProjectileProducer() {}

int ProjectileProducer::id() const {
	return this->unit_data.id0;
}

std::string ProjectileProducer::name() const {
	return this->unit_data.name;
}

void ProjectileProducer::initialise(Unit *unit, Player &) {

	// initialize graphic set
	unit->unit_type = this;
	unit->unit_class = this->unit_data.unit_class;
	unit->graphics = &this->graphics;

	// projectile speed
	coord::phys_t sp = this->unit_data.speed * coord::settings::phys_per_tile / 666;
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
	u->make_location<RadialObject>(this->unit_data.radius_size1, this->terrain_outline);

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
		for (coord::tile check_pos : tile_list(obj_ptr->get_range(pos))) {
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

	u->location->draw = [u]() {
		u->draw();
	};

	// try to place the obj, it knows best whether it will fit.
	if (u->location->place(terrain, init_pos, object_state::placed_no_collision)) {
		return u->location.get();
	}
	return nullptr;
}

} /* namespace openage */
