// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "game_save.h"

#include <fstream>
#include <vector>

#include "unit/producer.h"
#include "unit/unit.h"
#include "unit/unit_type.h"
#include "game_main.h"
#include "game_save.h"
#include "game_spec.h"

namespace openage {
namespace gameio {

void save_unit(std::ofstream &file, Unit *unit) {
	file << unit->unit_type->id() << std::endl;
	file << unit->get_attribute<attr_type::owner>().player.player_number << std::endl;
	coord::tile pos = unit->location->pos.start;
	file << pos.ne << " " << pos.se << std::endl;
}

void load_unit(std::ifstream &file, openage::GameMain *game) {
	int pr_id;
	int player_no;
	coord::phys_t ne, se;
	file >> pr_id;
	file >> player_no;
	file >> ne;
	file >> se;

	UnitType &saved_type = *game->get_settings()->spec->get_type(pr_id);
	game->placed_units.new_unit(saved_type, game->players[player_no], coord::tile{ne, se}.to_phys2().to_phys3());
}

void save_tile_content(std::ofstream &file, openage::TileContent *content) {
	file << content->terrain_id << std::endl;
	file << content->obj.size() << std::endl;
}

TileContent load_tile_content(std::ifstream &file) {
	openage::TileContent content;
	file >> content.terrain_id;

	unsigned int o_size;
	file >> o_size;
	return content;
}

void save(openage::GameMain *game, std::string fname) {
	std::ofstream file(fname, std::ofstream::out);

	// how many chunks
	std::vector<coord::chunk> used = game->terrain->used_chunks();
	file << used.size() << std::endl;

	// save each chunk
	for (coord::chunk &position : used) {
		file << position.ne << " " << position.se << std::endl;
		openage::TerrainChunk *chunk = game->terrain->get_chunk(position);

		file << chunk->tile_count << std::endl;
		for (size_t p = 0; p < chunk->tile_count; ++p) {
			save_tile_content( file, chunk->get_data(p) );
		}
	}

	// save units
	std::vector<openage::Unit *> units = game->placed_units.all_units();
	file << units.size() << std::endl;
	for (Unit *u : units) {
		save_unit( file, u );
	}
}

void load(openage::GameMain *game, std::string fname) {
	std::ifstream file(fname, std::ifstream::in);

	unsigned int num_chunks;
	file >> num_chunks;
	for (unsigned int c = 0; c < num_chunks; ++c) {
		coord::chunk_t ne, se;
		size_t tile_count;
		file >> ne;
		file >> se;
		file >> tile_count;
		openage::TerrainChunk *chunk = game->terrain->get_create_chunk(coord::chunk{ne, se});
		for (size_t p = 0; p < tile_count; ++p) {
			*chunk->get_data(p) = load_tile_content( file );
		}
	}

	game->placed_units.reset();
	unsigned int num_units;
	file >> num_units;
	for (unsigned int u = 0; u < num_units; ++u) {
		load_unit( file, game );
	}
}

} //namespace gameio
} //namespace openage
