// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "../gamedata/unit.gen.h"
#include "../terrain/terrain.h"
#include "../terrain/terrain_object.h"
#include "../terrain/terrain_outline.h"
#include "../util/strings.h"
#include "../datamanager.h"
#include "../log/log.h"
#include "ability.h"
#include "action.h"
#include "producer.h"
#include "unit.h"
#include "unit_texture.h"

namespace openage {

std::unordered_set<terrain_t> allowed_terrains(const gamedata::ground_type &restriction) {
	std::unordered_set<terrain_t> result;

	// 1, 14, and 15 are water, 2 is shore
	if (restriction == gamedata::ground_type::WATER ||
	    restriction == gamedata::ground_type::WATER_0x0D ||
	    restriction == gamedata::ground_type::WATER_SHIP_0x03 ||
	    restriction == gamedata::ground_type::WATER_SHIP_0x0F ||
	    restriction == gamedata::ground_type::WATER_ANY_0x11 ||
	    restriction == gamedata::ground_type::WATER_ANY_0x14) {
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

ObjectProducer::ObjectProducer(DataManager &dm, const gamedata::unit_object *ud)
	:
	datamanager(dm),
	unit_data(*ud),
	terrain_outline{nullptr},
	default_tex{dm.get_unit_texture(ud->graphic_standing0)},
	dead_unit_producer{dm.get_producer(ud->dead_unit_id)} {

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
	on_create = dm.get_sound(creation_sound);
	on_destroy = dm.get_sound(dying_sound);

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
	this->graphics[graphic_type::standing] = dm.get_unit_texture(this->unit_data.graphic_standing0);
	auto dying_tex = dm.get_unit_texture(this->unit_data.graphic_dying0);
	if (dying_tex) {
		this->graphics[graphic_type::dying] = dying_tex;
	}

	// default extra graphics
	this->graphics[graphic_type::attack] = this->graphics[graphic_type::standing];
	this->graphics[graphic_type::work] = this->graphics[graphic_type::standing];
	this->graphics[graphic_type::carrying] = this->graphics[graphic_type::standing];

	// pull extra graphics from unit commands
	auto cmds = dm.get_command_data(this->unit_data.id0);
	for (auto cmd : cmds) {
		if (cmd->action_graphic_id == -1 && cmd->proceed_graphic_id > 0) {
			this->graphics[graphic_type::work] = dm.get_unit_texture(cmd->proceed_graphic_id);
		}
		if (cmd->action_graphic_id > 0 && cmd->proceed_graphic_id > 0 ) {
			this->graphics[graphic_type::attack] = dm.get_unit_texture(cmd->proceed_graphic_id);
			this->graphics[graphic_type::work] = dm.get_unit_texture(cmd->action_graphic_id);
		}
		if (cmd->carrying_graphic_id > 0) {
			this->graphics[graphic_type::carrying] = dm.get_unit_texture(cmd->carrying_graphic_id);
		}
	}

}

ObjectProducer::~ObjectProducer() {}

int ObjectProducer::producer_id() const {
	return this->unit_data.id0;
}

std::string ObjectProducer::producer_name() const {
	return this->unit_data.name;
}

void ObjectProducer::initialise(Unit *unit, Player &player) {

	// log attributes
	unit->log(MSG(dbg) << "creating " <<
		this->unit_data.id0 << " (" <<
		this->unit_data.id1 << " " <<
		this->unit_data.id2 << ") " <<
		this->unit_data.name);

	// reset existing attributes
	unit->reset();

	// initialise unit
	unit->producer = this;
	unit->unit_class = this->unit_data.unit_class;
	unit->graphics = &this->graphics;

	// colour
	unit->add_attribute(new Attribute<attr_type::owner>(player));

	// hitpoints if available
	if (this->unit_data.hit_points > 0) {
		unit->add_attribute(new Attribute<attr_type::hitpoints>(this->unit_data.hit_points));
	}
	
	// collectable resources
	if (this->unit_data.unit_class == gamedata::unit_classes::TREES) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::wood, 125));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::BERRY_BUSH) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::food, 100));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::SEA_FISH) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::food, 200));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::PREY_ANIMAL) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::food, 140));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::SHEEP) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::food, 100));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::GOLD_MINE) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::gold, 800));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::STONE_MINE) {
		unit->add_attribute(new Attribute<attr_type::resource>(game_resource::stone, 350));
	}
	
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
	else {
		unit->push_action(std::make_unique<DeadAction>(unit), true);
	}

	// the default standing graphic
	unit->push_action(std::make_unique<IdleAction>(unit), true);

	// give required abilitys
	for (auto &a : this->type_abilities) {
		unit->give_ability(a);
	}
}

TerrainObject *ObjectProducer::place(Unit *u, Terrain &terrain, coord::phys3 init_pos) {

	// create new object with correct base shape
	TerrainObject *obj = nullptr;
	if (this->unit_data.selection_shape > 1) {
		obj = new RadialObject(*u, this->unit_data.radius_size0, this->terrain_outline.get());
	}
	else {
		obj = new SquareObject(*u, this->foundation_size, this->terrain_outline.get());
	}

	// find set of allowed terrains
	std::unordered_set<terrain_t> terrains = allowed_terrains(this->unit_data.terrain_restriction);

	/*
	 * decide what terrain is passable using this lambda
	 * currently unit avoids water and tiles with another unit
	 * this function should be true if pos is a valid position of the object
	 */
	obj->passable = [obj, &terrain, terrains](const coord::phys3 &pos) -> bool {

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(obj->get_range(pos))) {
			TileContent *tc = terrain.get_data(check_pos);

			// invalid tile types
			if (!tc || terrains.count(tc->terrain_id) == 0) {
				return false;
			}

			// ensure no intersections with other objects
			for (auto obj_cmp : tc->obj) {
				if (obj != obj_cmp && 
				    obj_cmp->check_collisions &&
				    obj->intersects(obj_cmp, pos)) {
					return false;
				}
			}
		}
		return true;
	};

	obj->draw = [=]() {
		if (u->selected) {
			obj->draw_outline();
		}
		u->draw();
	};

	// try to place the obj, it knows best whether it will fit.
	bool obj_placed = obj->place(&terrain, init_pos);
	if (obj_placed) {
		if (this->on_create) {
			this->on_create->play();
		}
		return obj;
	}

	// placing at the given position failed
	u->log(MSG(dbg) << "failed to place object");
	delete obj;
	return nullptr;
}

UnitTexture *ObjectProducer::default_texture() {
	return this->default_tex.get();
}


MovableProducer::MovableProducer(DataManager &dm, const gamedata::unit_movable *um)
	:
	ObjectProducer(dm, um),
	unit_data(*um),
	on_move{dm.get_sound(this->unit_data.move_sound)},
	on_attack{dm.get_sound(this->unit_data.move_sound)},
	projectile{dm.get_producer(this->unit_data.projectile_unit_id)} {

	// extra graphics
	this->graphics[graphic_type::walking] = dm.get_unit_texture(this->unit_data.walking_graphics0);
	this->graphics[graphic_type::attack] = dm.get_unit_texture(this->unit_data.attack_graphic);

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
	unit->add_attribute(new Attribute<attr_type::direction>(coord::phys3_delta{ 1, 0, 0 }));

	/*
	 * distance per millisecond -- consider original game speed
	 * where 1.5 in game seconds pass in 1 real second
	 */
	coord::phys_t sp = this->unit_data.speed * coord::settings::phys_per_tile / 666; 
	unit->add_attribute(new Attribute<attr_type::speed>(sp));

	// projectile of melee attacks
	if (this->unit_data.projectile_unit_id > 0 && this->projectile) {

		// calculate requirements for ranged attacks
		coord::phys_t range_phys = coord::settings::phys_per_tile * this->unit_data.max_range;
		unit->add_attribute(new Attribute<attr_type::attack>(this->projectile, range_phys, 48000, 1, this->graphics));
	}
	else {
		unit->add_attribute(new Attribute<attr_type::attack>(nullptr, 0, 0, 1, this->graphics));
	}
}

TerrainObject *MovableProducer::place(Unit *unit, Terrain &terrain, coord::phys3 init_pos) {
	return ObjectProducer::place(unit, terrain, init_pos);
}

LivingProducer::LivingProducer(DataManager &dm, const gamedata::unit_living *ud)
	:
	MovableProducer(dm, ud),
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
		unit->add_attribute(new Attribute<attr_type::gatherer>());

		// add graphic ids for resource actions
		auto &gather_attr = unit->get_attribute<attr_type::gatherer>();
		gather_attr.current_type = game_resource::wood;
		gather_attr.capacity = 10.0f;
		gather_attr.gather_rate = 0.002f;

		// currently not sure where the game data keeps these values
		// todo PREY_ANIMAL SEA_FISH
		if (this->unit_data.id0 == 83) {

			// male graphics
			gather_attr.graphics[gamedata::unit_classes::BUILDING] = this->datamanager.get_producer(156); // builder 118
			gather_attr.graphics[gamedata::unit_classes::BERRY_BUSH] = this->datamanager.get_producer(120); // forager
			gather_attr.graphics[gamedata::unit_classes::SHEEP] = this->datamanager.get_producer(592); // sheperd
			gather_attr.graphics[gamedata::unit_classes::TREES] = this->datamanager.get_producer(123); // woodcutter
			gather_attr.graphics[gamedata::unit_classes::GOLD_MINE] = this->datamanager.get_producer(579); // gold miner
			gather_attr.graphics[gamedata::unit_classes::STONE_MINE] = this->datamanager.get_producer(124); // stone miner
			
		}
		else {

			// female graphics
			gather_attr.graphics[gamedata::unit_classes::BUILDING] = this->datamanager.get_producer(222); // builder 212
			gather_attr.graphics[gamedata::unit_classes::BERRY_BUSH] = this->datamanager.get_producer(354); // forager
			gather_attr.graphics[gamedata::unit_classes::SHEEP] = this->datamanager.get_producer(590); // sheperd
			gather_attr.graphics[gamedata::unit_classes::TREES] = this->datamanager.get_producer(218); // woodcutter
			gather_attr.graphics[gamedata::unit_classes::GOLD_MINE] = this->datamanager.get_producer(581); // gold miner
			gather_attr.graphics[gamedata::unit_classes::STONE_MINE] = this->datamanager.get_producer(220); // stone miner
		}
		unit->give_ability(std::make_shared<GatherAbility>(this->on_attack));
		unit->give_ability(std::make_shared<BuildAbility>(this->on_attack));
	}
	else if (this->unit_data.unit_class == gamedata::unit_classes::FISHING_BOAT) {
		unit->add_attribute(new Attribute<attr_type::gatherer>());

		// add fishing abilites
		auto &gather_attr = unit->get_attribute<attr_type::gatherer>();
		gather_attr.current_type = game_resource::food;
		gather_attr.capacity = 15.0f;
		gather_attr.gather_rate = 0.002f;
		gather_attr.graphics[gamedata::unit_classes::SEA_FISH] = this;

		unit->give_ability(std::make_shared<GatherAbility>(this->on_attack));
	}
}

TerrainObject *LivingProducer::place(Unit *unit, Terrain &terrain, coord::phys3 init_pos) {
	return MovableProducer::place(unit, terrain, init_pos);
}

BuldingProducer::BuldingProducer(DataManager &dm, const gamedata::unit_building *ud)
	:
	datamanager(dm),
	unit_data{*ud},
	texture{dm.get_unit_texture(ud->graphic_standing0)},
	destroyed{dm.get_unit_texture(ud->graphic_dying0)},
	trainable1{dm.get_producer(83)}, // 83 = m villager 
	trainable2{dm.get_producer(293)}, // 293 = f villager 
	projectile{dm.get_producer(this->unit_data.projectile_unit_id)},
	foundation_terrain{ud->terrain_id} {

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
	on_create = dm.get_sound(creation_sound);
	on_destroy = dm.get_sound(dying_sound);

	// convert the float to the discrete foundation size...
	this->foundation_size = {
		(int)(this->unit_data.radius_size0 * 2),
		(int)(this->unit_data.radius_size1 * 2),
	};
	
	// graphic set
	this->graphics[graphic_type::construct] = dm.get_unit_texture(ud->construction_graphic_id);
	this->graphics[graphic_type::standing] = dm.get_unit_texture(ud->graphic_standing0);
	this->graphics[graphic_type::attack] = dm.get_unit_texture(ud->graphic_standing0);
	auto dying_tex = dm.get_unit_texture(ud->graphic_dying0);
	if (dying_tex) {
		this->graphics[graphic_type::dying] = dying_tex;
	}

	this->terrain_outline = square_outline(this->foundation_size);
}

BuldingProducer::~BuldingProducer() {}

int BuldingProducer::producer_id() const {
	return this->unit_data.id0;
}

std::string BuldingProducer::producer_name() const {
	return this->unit_data.name;
}

void BuldingProducer::initialise(Unit *unit, Player &player) {

	// initialize graphic set
	unit->producer = this;
	unit->unit_class = this->unit_data.unit_class;
	unit->graphics = &this->graphics;

	auto player_attr = new Attribute<attr_type::owner>(player);
	unit->add_attribute(player_attr);
	unit->add_attribute(new Attribute<attr_type::building>());
	unit->add_attribute(new Attribute<attr_type::garrison>());
	unit->add_attribute(new Attribute<attr_type::hitpoints>(this->unit_data.hit_points));

	// basic attributes on annexes
	for (auto obj : unit->location->get_children()) {
		obj->unit.add_attribute(player_attr);
		obj->unit.push_action(std::make_unique<IdleAction>(&obj->unit), true);
	}

	auto &bl_attr = unit->get_attribute<attr_type::building>();
	bl_attr.pp = trainable2;

	// if a destroyed texture exists
	if (this->destroyed) {
		unit->push_action(std::make_unique<DeadAction>(unit), true);
	}
	unit->push_action(std::make_unique<IdleAction>(unit), true);

	if (this->unit_data.projectile_unit_id > 0 && this->projectile) {
		coord::phys_t range_phys = coord::settings::phys_per_tile * this->unit_data.max_range;
		unit->add_attribute(new Attribute<attr_type::attack>(this->projectile, range_phys, 350000, 1, this->graphics));
		unit->give_ability(std::make_shared<AttackAbility>());
	}

	// building can train new units and ungarrison
	unit->give_ability(std::make_shared<TrainAbility>());
	unit->give_ability(std::make_shared<UngarrisonAbility>());
}

TerrainObject *BuldingProducer::place(Unit *u, Terrain &terrain, coord::phys3 init_pos) {

	// not a tc --- hacks / fix me
	bool collisions = this->unit_data.id0 != 109;

	// buildings have a square base
	TerrainObject *obj = new SquareObject(*u, this->foundation_size, this->terrain_outline, collisions);

	/*
	 * decide what terrain is passable using this lambda
	 * currently unit avoids water and tiles with another unit
	 * this function should be true if pos is a valid position of the object
	 */
	obj->passable = [obj, &terrain](const coord::phys3 &pos) -> bool {

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(obj->get_range(pos))) {
			TileContent *tc = terrain.get_data(check_pos);
			if (!tc) return false;
			for (auto tobj : tc->obj) {
				if (tobj->check_collisions) return false;
			}
		}
		return true;
	};

	// drawing function
	obj->draw = [=]() {
		if (u->selected && collisions) {
			obj->draw_outline();
		}
		u->draw();
	};

	// try to place the obj, it knows best whether it will fit.
	bool obj_placed = obj->place(&terrain, init_pos);
	if (!obj_placed) {
		delete obj;
		return nullptr;
	}

	// annex objects
	for (unsigned i = 0; i < 4; ++i) {
		const gamedata::building_annex &annex = this->unit_data.building_annex.data[i];
		if (annex.unit_id > 0) {

			// make objects for annex	
			coord::phys3 a_pos = obj->pos.draw;
			a_pos.ne += annex.misplaced0 * coord::settings::phys_per_tile;
			a_pos.se += annex.misplaced1 * coord::settings::phys_per_tile;
			TerrainObject *annex_obj = this->make_annex(*u, terrain, annex.unit_id, a_pos, i == 0);
			obj->annex(annex_obj);
		}
	}

	if (this->on_create) {
		this->on_create->play();
	}
	if (this->foundation_terrain > 0) {
		// TODO: use the gamedata terrain lookup!
		obj->set_ground(this->foundation_terrain, 0);
	}
	return obj;
}

UnitTexture *BuldingProducer::default_texture() {
	return this->texture.get();
}

TerrainObject *BuldingProducer::make_annex(Unit &u, Terrain &t, int annex_id, coord::phys3 annex_pos, bool c) {

	// find annex foundation size
	auto b = datamanager.get_building_data(annex_id);
	coord::tile_delta annex_foundation = {
		static_cast<int>(b->radius_size0 * 2),
		static_cast<int>(b->radius_size1 * 2),
	};

	// producers place by the nw tile
	auto p = datamanager.get_producer(annex_id);
	coord::phys3 start_tile = annex_pos;
	start_tile.ne -= b->radius_size0 * coord::settings::phys_per_tile;
	start_tile.se -= b->radius_size1 * coord::settings::phys_per_tile;

	// place on terrain
	// TODO: less hacks
	Unit *annex_unit = u.get_container()->new_unit().get();
	annex_unit->producer = p;
	annex_unit->graphics = &p->graphics;
	TerrainObject *annex_obj = new SquareObject(*annex_unit, annex_foundation, c);
	annex_obj->place(&t, start_tile);

	if (c) {
		annex_obj->draw = [annex_obj, annex_pos, p, &u]() {

			// draws the outline in the right order
			if (u.selected) {
				annex_obj->get_parent()->draw_outline();
			}

			if (u.has_attribute(attr_type::building) &&
			    u.get_attribute<attr_type::building>().completed >= 1.0f) {
				annex_obj->unit.draw();
			}
			
		};
	}
	else {
		annex_obj->draw = [annex_obj, annex_pos, p, &u]() {
			if (u.has_attribute(attr_type::building) &&
			    u.get_attribute<attr_type::building>().completed >= 1.0f) {
				annex_obj->unit.draw();
			}
		};
	}

	return annex_obj;
}

ProjectileProducer::ProjectileProducer(DataManager &dm, const gamedata::unit_projectile *pd)
	:
	unit_data{*pd},
	tex{dm.get_unit_texture(this->unit_data.graphic_standing0)},
	sh{dm.get_unit_texture(3379)}, // 3379 = general arrow shadow
	destroyed{dm.get_unit_texture(this->unit_data.graphic_dying0)} {

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

int ProjectileProducer::producer_id() const {
	return this->unit_data.id0;
}

std::string ProjectileProducer::producer_name() const {
	return this->unit_data.name;
}

void ProjectileProducer::initialise(Unit *unit, Player &) {

	// initialize graphic set
	unit->producer = this;
	unit->unit_class = this->unit_data.unit_class;
	unit->graphics = &this->graphics;

	// projectile speed
	coord::phys_t sp = this->unit_data.speed * coord::settings::phys_per_tile / 666;
	unit->add_attribute(new Attribute<attr_type::speed>(sp));
	unit->add_attribute(new Attribute<attr_type::projectile>(this->unit_data.projectile_arc));
	unit->add_attribute(new Attribute<attr_type::direction>(coord::phys3_delta{ 1, 0, 0 }));

	// if destruction graphic is available
	if (this->destroyed) {
		unit->push_action(std::make_unique<DeadAction>(unit), true);
	}
}

TerrainObject *ProjectileProducer::place(Unit *u, Terrain &terrain, coord::phys3 init_pos) {
	/*
	 * radial base shape without collision checking
	 */
	TerrainObject *obj = new RadialObject(*u, this->unit_data.radius_size1, this->terrain_outline, false);

	obj->passable = [obj, u, &terrain](const coord::phys3 &pos) -> bool {
		if (pos.up > 64000) {
			return true;
		}

		// avoid intersections with launcher
		Unit *launcher = nullptr;
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
		for (coord::tile check_pos : tile_list(obj->get_range(pos))) {
			TileContent *tc = terrain.get_data(check_pos);
			if (!tc) return false;

			// ensure no intersections with other objects
			for (auto obj_cmp : tc->obj) {
				if (obj != obj_cmp && 
					&obj_cmp->unit != launcher &&
				    obj_cmp->check_collisions &&
				    obj->intersects(obj_cmp, pos)) {
					return false;
				}
			}
		}
		return true;
	};

	obj->draw = [=]() {
		u->draw();
	};


	// try to place the obj, it knows best whether it will fit.
	bool obj_placed = obj->place(&terrain, init_pos);
	if (obj_placed) {
		return obj;
	}
	delete obj;
	return nullptr;
}

UnitTexture *ProjectileProducer::default_texture() {
	return this->tex.get();
}

} /* namespace openage */
