// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "../terrain/terrain.h"
#include "../terrain/terrain_object.h"
#include "../terrain/terrain_outline.h"
#include "../util/strings.h"
#include "../game.h"
#include "../log.h"
#include "ability.h"
#include "action.h"
#include "producer.h"
#include "unit.h"

namespace openage {

UnitTypeTest::UnitTypeTest(const gamedata::unit_living *ud,
                           Texture *deadt,
                           Texture *idlet,
                           Texture *movet,
                           Texture *attackt,
                           TestSound *creates,
                           TestSound *destroys,
                           TestSound *moves,
                           TestSound *attacks)
	:
	unit_data{*ud},
	terrain_outline{radial_outline(ud->radius_size1)},
	dead{deadt},
	idle{idlet},
	moving{movet},
	attacking{attackt},
	on_create{creates},
	on_destroy{destroys},
	on_move{moves},
	on_attack{attacks} {
}

UnitTypeTest::~UnitTypeTest() {

}

void UnitTypeTest::initialise(Unit *unit) {
	/*
	 * log attributes
	 */
	log::dbg("creating %d (%d %d) %s", this->unit_data.id0, this->unit_data.id1, this->unit_data.id2, this->unit_data.name.c_str());

	/*
	 * basic attributes
	 */
	unit->add_attribute(new Attribute<attr_type::color>(util::random_range(1, 8 + 1)));
	unit->add_attribute(new Attribute<attr_type::hitpoints>(50, 50));
	unit->add_attribute(new Attribute<attr_type::direction>(coord::phys3_delta{1, 0, 0}));

	/*
	 * distance per millisecond -- consider game speed
	 */
	coord::phys_t sp = this->unit_data.speed * (1 << 16) / 500;
	unit->add_attribute(new Attribute<attr_type::speed>(sp));

	/*
	 * Initial action stack
	 */
	unit->push_action(std::make_unique<DeadAction>(unit, this->dead,
	                                               this->on_destroy));
	unit->push_action(std::make_unique<IdleAction>(unit, this->idle));

	unit->give_ability(std::make_unique<MoveAbility>(this->moving, this->on_move));
	unit->give_ability(std::make_unique<AttackAbility>(this->attacking, this->on_attack));

	if (this->unit_data.unit_class == gamedata::unit_classes::CIVILIAN) {
		unit->give_ability(std::make_unique<GatherAbility>(this->attacking, this->on_attack));
	}
}

bool UnitTypeTest::place(Unit *unit, Terrain *terrain, coord::tile init_tile) {

	/*
	 * decide what terrain is passable using this lambda
	 * currently unit avoids water and tiles with another unit
	 * this function should be true if pos is a valid position of the object
	 */
	auto passable = [=](const coord::phys3 &pos) -> bool {

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(unit->location->get_range(pos))) {
			TileContent *tc = terrain->get_data(check_pos);
			if (!tc) return false;

			// not allowed on water
			if (tc->terrain_id == 1 || tc->terrain_id == 14 || tc->terrain_id == 15) return false;

			// ensure no intersections with other objects
			for (auto obj_cmp : tc->obj) {
				if (unit->location != obj_cmp && unit->location->intersects(obj_cmp, pos)) {
					return false;
				}
			}
		}
		return true;
	};

	/*
	 * radial base shape
	 */
	unit->location = new RadialObject(unit, passable, this->unit_data.radius_size1, this->terrain_outline);


	// try to place the obj, it knows best whether it will fit.
	coord::phys3 init_pos = init_tile.to_phys2().to_phys3();
	bool obj_placed = unit->location->place(terrain, init_pos);
	if (obj_placed) {
		this->on_create->play();
	}
	return obj_placed;
}

Texture *UnitTypeTest::default_texture() {
	return idle;
}

BuldingProducer::BuldingProducer(Texture *tex,
                                 coord::tile_delta foundation_size,
                                 int foundation,
                                 TestSound *create,
                                 TestSound *destroy)
	:
	on_create{create},
	on_destroy{destroy},
	terrain_outline{square_outline(foundation_size)},
	texture{tex},
	size(foundation_size),
	foundation_terrain{foundation} {
}

BuldingProducer::~BuldingProducer() {
}

void BuldingProducer::initialise(Unit *unit) {
	unit->add_attribute(new Attribute<attr_type::color>(util::random_range(1, 8 + 1)));
	unit->add_attribute(new Attribute<attr_type::dropsite>());

	unit->push_action(std::make_unique<DeadAction>(unit, this->texture,
	                                               this->on_destroy));
	unit->push_action(std::make_unique<IdleAction>(unit, this->texture));
}

bool BuldingProducer::place(Unit *unit, Terrain *terrain, coord::tile init_tile) {

	/*
	 * decide what terrain is passable using this lambda
	 * currently unit avoids water and tiles with another unit
	 * this function should be true if pos is a valid position of the object
	 */
	auto passable = [=](const coord::phys3 &pos) -> bool {

		// look at all tiles in the bases range
		for (coord::tile check_pos : tile_list(unit->location->get_range(pos))) {
			TileContent *tc = terrain->get_data(check_pos);
			if (!tc) return false;
			if (!tc->obj.empty()) return false;
		}
		return true;
	};

	// buildings have a square baase
	unit->location = new SquareObject(unit, passable, this->size, this->terrain_outline);

	// try to place the obj, it knows best whether it will fit.
	coord::phys3 init_pos = init_tile.to_phys2().to_phys3();
	bool obj_placed = unit->location->place(terrain, init_pos);
	if (obj_placed) {
		this->on_create->play();

		if (this->foundation_terrain > 0) {
			// TODO: use the gamedata terrain lookup!
			unit->location->set_ground(this->foundation_terrain, 0);
		}
	}
	return obj_placed;
}

Texture *BuldingProducer::default_texture() {
	return this->texture;
}

ProducerLoader::ProducerLoader(Game *m)
	:
	main(m) {}

std::unique_ptr<UnitProducer> ProducerLoader::load_building(const gamedata::unit_building &building) {
	auto tex = this->main->find_graphic(building.graphic_standing0);

	if (tex == nullptr) {
		return nullptr;
	}

	// convert the float to the discrete foundation size...
	coord::tile_delta foundation_size = {
		(int)(building.radius_size0 * 2),
		(int)(building.radius_size1 * 2),
	};

	// find suitable sounds
	int creation_sound = building.sound_creation0;
	int dying_sound = building.sound_dying;
	if (creation_sound == -1) {
		creation_sound = building.sound_creation1;
	}
	if (creation_sound == -1) {
		creation_sound = building.sound_selection;
	}
	if (dying_sound == -1) {
		dying_sound = 323; //generic explosion sound
	}

	// make and return producer
	return std::make_unique<BuldingProducer>(
		tex,
		foundation_size,
		building.terrain_id,
		this->main->find_sound(creation_sound),
		this->main->find_sound(dying_sound)
	);
}

std::unique_ptr<UnitProducer> ProducerLoader::load_living(const gamedata::unit_living &unit) {

	// find and load graphics
	auto tex_die    = this->main->find_graphic(unit.graphic_dying0);
	auto tex_stand  = this->main->find_graphic(unit.graphic_standing0);
	auto tex_walk   = this->main->find_graphic(unit.walking_graphics0);
	auto tex_attack = this->main->find_graphic(unit.attack_graphic);

	// check graphics exist
	if (not (tex_die && tex_stand && tex_walk && tex_attack)) {
		return nullptr;
	}

	// get required sounds
	TestSound *snd_create  = this->main->find_sound(unit.sound_creation0);
	TestSound *snd_destroy = this->main->find_sound(unit.sound_dying);
	TestSound *snd_move    = this->main->find_sound(unit.move_sound);

	return std::make_unique<UnitTypeTest>(
		&unit,
		tex_die, tex_stand, tex_walk, tex_attack,
		snd_create, snd_destroy, snd_move, snd_move
	);
}

std::unique_ptr<UnitProducer> ProducerLoader::load_object(const gamedata::unit_object &object) {
	Texture *tex = this->main->find_graphic(object.graphic_standing0);
	if ( !tex ) {
		return nullptr;
	}

	// convert the float to the discrete foundation size...
	openage::coord::tile_delta foundation_size = {
		(int)(object.radius_size0 * 2),
		(int)(object.radius_size1 * 2),
	};

	// find suitable sounds
	int creation_sound = object.sound_creation0;
	int dying_sound = object.sound_dying;
	if (creation_sound == -1) {
		creation_sound = object.sound_creation1;
	}
	if (creation_sound == -1) {
		creation_sound = object.sound_selection;
	}
	if (dying_sound == -1) {
		dying_sound = 323; //generic explosion sound
	}

	// make and return producer
	return std::make_unique<BuldingProducer>(
		tex,
		foundation_size,
		0,
		this->main->find_sound(creation_sound),
		this->main->find_sound(dying_sound)
	);
}

std::vector<std::unique_ptr<UnitProducer>> ProducerLoader::create_producers(const std::vector<gamedata::empiresdat> &gamedata, int your_civ_id) {
	std::vector<std::unique_ptr<UnitProducer>> producer_objects;

	// create building unit types
	for (auto &building : gamedata[0].civs.data[your_civ_id].units.building.data) {
		std::unique_ptr<UnitProducer> producer = this->load_building(building);
		if (producer) producer_objects.emplace_back(std::move(producer));
	}

	// create living unit types
	for (auto &unit : gamedata[0].civs.data[your_civ_id].units.living.data) {
		std::unique_ptr<UnitProducer> producer = this->load_living(unit);
		if (producer) producer_objects.emplace_back(std::move(producer));
	}

	// create object unit types
	for (auto &obj : gamedata[0].civs.data[0].units.object.data) {
		std::unique_ptr<UnitProducer> producer = this->load_object(obj);
		if (producer) producer_objects.emplace_back(std::move(producer));
	}
	return producer_objects;
}

} /* namespace openage */
