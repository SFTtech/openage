// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "unit/unit.h"
#include "game_save.h"
#include "game_spec.h"
#include "generator.h"

namespace openage {

coord::tile random_tile(rng::RNG &rng, tileset_t tiles) {
	if (tiles.empty()) {
		log::log(MSG(err) << "random tile failed");
		return coord::tile{0, 0};
	}
	uint64_t index = rng.random() % tiles.size();
	auto it = std::begin(tiles);
	std::advance(it, index);
	return *it;
}


Region::Region(int size)
	:
	owner{0},
	object_id{0},
	terrain_id{0},
	center{0, 0} {
	for (int ne = -size; ne < size; ++ne) {
		for (int se = -size; se < size; ++se) {
			this->tiles.emplace(coord::tile{ne, se});
		}
	}
}

Region::Region(coord::tile center, tileset_t tiles)
	:
	owner{0},
	object_id{0},
	terrain_id{0},
	center(center),
	tiles{tiles} {
}

tileset_t Region::get_tiles() const {
	return this->tiles;
}

coord::tile Region::get_center() const {
	return this->center;
}

coord::tile Region::get_tile(rng::RNG &rng) const {
	return random_tile(rng, this->tiles);
}


tileset_t Region::subset(rng::RNG &rng, coord::tile tile, unsigned int number) const {
	std::unordered_set<coord::tile> subtiles;
	subtiles.emplace(tile);

	// outside layer of tiles
	std::unordered_set<coord::tile> edge_set;

	while (subtiles.size() < number) {
		if (edge_set.empty()) {

			// try fill the edge list
			for (auto &t : subtiles) {
				coord::tile a{t.ne + 1, t.se};
				if (this->tiles.count(a) && !subtiles.count(a)) {
					edge_set.emplace(a);
				}
				coord::tile b{t.ne, t.se + 1};
				if (this->tiles.count(b) && !subtiles.count(b)) {
					edge_set.emplace(b);
				}
				coord::tile c{t.ne - 1, t.se};
				if (this->tiles.count(c) && !subtiles.count(c)) {
					edge_set.emplace(c);
				}
				coord::tile d{t.ne, t.se - 1};
				if (this->tiles.count(d) && !subtiles.count(d)) {
					edge_set.emplace(d);
				}
			}
			if (edge_set.empty()) {

				// unable to grow further
				return subtiles;
			}
		}

		// transfer a random tile
		coord::tile next_tile = random_tile(rng, edge_set);
		edge_set.erase(next_tile);
		subtiles.emplace(next_tile);
	}
	return subtiles;
}

Region Region::take_tiles(rng::RNG &rng, coord::tile tile, unsigned int number) {

	// take a random subset
	tileset_t new_set = this->subset(rng, tile, number);

	// erase from current set
	for (auto &t: new_set) {
		this->tiles.erase(t);
	}

	Region new_region(tile, new_set);
	new_region.terrain_id = this->terrain_id;
	return new_region;
}

Region Region::take_random(rng::RNG &rng, unsigned int number) {
	return this->take_tiles(rng, this->get_tile(rng), number);
}

Generator::Generator(Engine *engine)
	:
	OptionNode{"Generator"},
	assetmanager{engine->get_data_dir()},
	spec{std::make_shared<GameSpec>(&this->assetmanager)} {

	// node settings
	this->set_parent(engine);
	this->add("generation_seed", 12345);
	this->add("terrain_size", 2);
	this->add("terrain_base_id", 0);
	this->add("player_area", 850);
	this->add("load_filename", "default_save.txt");
	this->add("player_names", options::option_list{"name1", "name2"});

	// Save game functions
	options::OptionAction save_action("save_game", [this]() {
		Engine &engine = Engine::get();
		if (!engine.get_game()) {
			log::log(MSG(warn) << "game required for saving");
			return;
		}
		auto filename = this->get_variable("load_filename").value<std::string>();
		gameio::save(engine.get_game(), filename);
	});
	this->add_action(save_action);

	// Load game function
	options::OptionAction load_action("load_game", [this]() {
		Engine &engine = Engine::get();
		if (engine.get_game()) {
			log::log(MSG(warn) << "close existing game before loading");
			return;
		}
		auto filename = this->get_variable("load_filename").value<std::string>();

		// create an empty game
		this->regions.clear();
		engine.start_game(*this);
		gameio::load(engine.get_game(), filename);
	});
	this->add_action(load_action);

	// create a game using this generator
	options::OptionAction start_action("generate_game", [this]() {
		this->create();
	});
	this->add_action(start_action);

	// stop game
	options::OptionAction end_action("end_game", [this]() {
		Engine &engine = Engine::get();
		engine.end_game();
	});
	this->add_action(end_action);
}


std::shared_ptr<GameSpec> Generator::get_spec() const {
	return this->spec;
}

std::vector<std::string> Generator::player_names() const {
	auto &var = this->get_variable("player_names");
	auto &names = var.value<options::option_list>();

	std::vector<std::string> result;
	result.push_back("gaia");
	for (auto &n : names) {
		result.push_back(n.str_value());
	}
	return result;
}

void Generator::create_regions() {

	// get option settings
	int seed = this->getv<int>("generation_seed");
	int size = this->getv<int>("terrain_size");
	int base_id = this->getv<int>("terrain_base_id");
	int p_area = this->getv<int>("player_area");

	rng::RNG rng(seed);
	Region base(size * 16);
	base.terrain_id = base_id;
	std::vector<Region> player_regions;
	std::vector<int> objs{59, 66, 102, 349, 351, 594};

	int player_count = this->player_names().size() - 1;
	for (int i = 0; i < player_count; ++i) {
		log::log(MSG(dbg) << "generate player " << i);
		double angle = static_cast<double>(i) / static_cast<double>(player_count);
		int ne = size * 11.0 * sin(2 * M_PI * angle);
		int se = size * 11.0 * cos(2 * M_PI * angle);
		coord::tile player_tile{ne, se};

		Region player = base.take_tiles(rng, player_tile, p_area);
		player.terrain_id = 10;

		Region obj_space = player.take_tiles(rng, player.get_center(), p_area / 5);
		obj_space.owner = i + 1;
		obj_space.terrain_id = 8;

		Region trees1 = player.take_random(rng, p_area / 10);
		trees1.terrain_id = 9;
		trees1.object_id = 349;

		Region trees2 = player.take_random(rng, p_area / 10);
		trees2.terrain_id = 9;
		trees2.object_id = 351;

		Region stone = player.take_random(rng, 5);
		stone.object_id = 102;

		Region gold = player.take_random(rng, 7);
		gold.object_id = 66;

		Region forage = player.take_random(rng, 6);
		forage.object_id = 59;

		Region sheep = player.take_random(rng, 4);
		sheep.owner = obj_space.owner;
		sheep.object_id = 594;

		player_regions.push_back(player);
		player_regions.push_back(obj_space);
		player_regions.push_back(trees1);
		player_regions.push_back(trees2);
		player_regions.push_back(stone);
		player_regions.push_back(gold);
		player_regions.push_back(forage);
		player_regions.push_back(sheep);
	}

	for (int i = 0; i < 6; ++i) {
		Region extra_trees = base.take_random(rng, 160);
		extra_trees.terrain_id = 9;
		extra_trees.object_id = 349;
		player_regions.push_back(extra_trees);
	}

	// set regions
	this->regions.clear();
	this->regions.push_back(base);
	for (auto &r : player_regions) {
		this->regions.push_back(r);
	}
}


std::shared_ptr<Terrain> Generator::terrain() const {
	auto terrain = std::make_shared<Terrain>(this->spec->get_terrain_meta(), true);
	for (auto &r : this->regions) {
		for (auto &tile : r.get_tiles()) {
			TerrainChunk *chunk = terrain->get_create_chunk(tile);
			chunk->get_data(tile)->terrain_id = r.terrain_id;
		}
	}
	return terrain;
}

void Generator::add_units(GameMain &m) const {
	for (auto &r : this->regions) {

		// Regions filled with resource objects
		// trees / mines
		if (r.object_id) {
			Player &p = m.players[r.owner];
			auto otype = this->spec->get_type(r.object_id);
			for (auto &tile : r.get_tiles()) {
				m.placed_units.new_unit(*otype, p, tile.to_tile3().to_phys3());
			}
		}

		// A space for starting town center and villagers
		else if (r.owner) {
			Player &p = m.players[r.owner];
			auto tctype = this->spec->get_type(109); // town center
			auto mvtype = this->spec->get_type(83);  // villager
			auto fvtype = this->spec->get_type(293); // villager
			coord::tile tile = r.get_center();
			tile.ne -= 1;
			tile.se -= 1;

			// Place a completed town center
	 		auto ref = m.placed_units.new_unit(*tctype, p, tile.to_tile3().to_phys3());
	 		complete_building(*ref.get());

	 		// Place three villagers
			tile.ne -= 1;
	 		m.placed_units.new_unit(*fvtype, p, tile.to_tile3().to_phys3());
	 		tile.se += 1;
	 		m.placed_units.new_unit(*mvtype, p, tile.to_tile3().to_phys3());
	 		tile.se += 1;
	 		m.placed_units.new_unit(*fvtype, p, tile.to_tile3().to_phys3());
		}
	}
}

void Generator::create() {
	if (!this->spec->load_complete()) {
		log::log(MSG(dbg) << "loading not completed");
		return;
	}

	// generation
	this->create_regions();

	// create main
	Engine &e = Engine::get();
	e.start_game(*this);
}


void lol() {
	// TODO: move these somewhere else
	//this->assetmanager.check_updates();
}

} // namespace openage