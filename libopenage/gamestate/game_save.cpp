// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "game_save.h"

#include <fstream>
#include <vector>

#include "../log/log.h"
#include "../unit/producer.h"
#include "../unit/unit.h"
#include "../unit/unit_type.h"
#include "game_main.h"
#include "game_save.h"
#include "game_spec.h"
#include "picojson.h"

namespace openage {
namespace gameio {

picojson::value save_unit(Unit *unit) {
	picojson::object unitj;
	unitj["type"]   =  picojson::value((double) unit->unit_type->id() );
	unitj["player"] =  picojson::value((double) unit->get_attribute<attr_type::owner>().player.player_number );

	//position
	coord::tile pos = unit->location->pos.start;
	unitj["position-se"] =  picojson::value((double) pos.ne );
	unitj["position-ne"] =  picojson::value((double) pos.se );

	//unit properties
	picojson::object properties;
	bool has_building_attr = unit->has_attribute(attr_type::building);
	if (has_building_attr) {
	  unitj["isbuilding"] = picojson::value(true);
	  properties["iscompleted"] = picojson::value(unit->get_attribute<attr_type::building>().completed);
	} else {
	  unitj["isbuilding"] = picojson::value(false);
	}
	unitj["properties"] = picojson::value(properties);
	return picojson::value(unitj);
}

void load_unit(std::ifstream &file, openage::GameMain *game) {
	int pr_id;
	int player_no;
	coord::phys_t ne, se;
	file >> pr_id;
	file >> player_no;
	file >> ne;
	file >> se;

	UnitType &saved_type = *game->get_player(player_no)->get_type(pr_id);
	auto ref = game->placed_units.new_unit(saved_type, game->players[player_no], coord::tile{ne, se}.to_phys2().to_phys3());

	bool has_building_attr;
	file >> has_building_attr;
	if (has_building_attr) {
		float completed;
		file >> completed;
		if (completed >= 1.0f && ref.is_valid()) {
			complete_building(*ref.get());
		}
	}
}

picojson::value save_tile_content( openage::TileContent *content) {
	picojson::object tile;
	tile["terrain-id"] = picojson::value((double) content->terrain_id);
	tile["size"]       = picojson::value((double) content->obj.size());
	return picojson::value(tile);
}
TileContent load_tile_content(std::ifstream &file) {
	openage::TileContent content;
	file >> content.terrain_id;

	unsigned int o_size;
	file >> o_size;
	return content;
}

void save(openage::GameMain *game, std::string fname) {
	log::log(MSG(dbg) << "saving " + fname);

	picojson::object savegame;
	//metadata
	savegame["save_label"] = picojson::value(save_label);
	savegame["version"]    = picojson::value(save_version);
	savegame["build"]      = picojson::value(config::version);

	// saving terrain
	picojson::object terrain;
	// how many chunks
	std::vector<coord::chunk> used = game->terrain->used_chunks();
	terrain["chunks-size"] = picojson::value( (double) used.size());
	picojson::array chunks;

	// loop through chunks
	for (coord::chunk &position : used) {
	  openage::TerrainChunk *chunk = game->terrain->get_chunk(position);
	  picojson::object chunkj;

	  //chunk metadata
	  chunkj["position-se"] = picojson::value((double) position.se);
	  chunkj["position-ne"] = picojson::value((double) position.ne);
	  chunkj["tile-count"] = picojson::value((double) chunk->tile_count);

	  // saving tiles
	  picojson::array tiles;
	  for (size_t p = 0; p < chunk->tile_count; ++p) {
	    tiles.push_back( save_tile_content( chunk->get_data(p) ));
	  }
	  chunkj["tiles"] = picojson::value(tiles);

	  chunks.push_back(picojson::value(chunkj));
	}
	terrain["chunks"] = picojson::value(chunks);
	savegame["terrain"] = picojson::value(terrain);

	// save units
	std::vector<openage::Unit *> units = game->placed_units.all_units();
	picojson::array unitsj;
	for (Unit *u : units) {
		unitsj.push_back(save_unit(u));
	}
	savegame["units"] = picojson::value(unitsj);

	// save to file
	std::ofstream file(fname, std::ofstream::out);
	file << picojson::value(savegame).serialize() << "\n";
}

void load(openage::GameMain *game, std::string fname) {
	std::ifstream file(fname, std::ifstream::in);
	if (!file.good()) {
		log::log(MSG(dbg) << "could not find " + fname);
		return;
	}
	log::log(MSG(dbg) << "loading " + fname);

	// load metadata
	std::string file_label;
	file >> file_label;
	if (file_label != save_label) {
		log::log(MSG(warn) << fname << " is not a savefile");
		return;
	}
	std::string version;
	file >> version;
	if (version != save_version) {
		log::log(MSG(warn) << "savefile has different version");
	}
	std::string build;
	file >> build;

	// read terrain chunks
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

}} // openage::gameio
