// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "game_save.h"

#include <fstream>
#include <vector>
#include <string>

#include "../engine.h"
#include "../log/log.h"
#include "../unit/producer.h"
#include "../unit/unit.h"
#include "../unit/unit_type.h"
#include "game_main.h"
#include "game_save.h"
#include "game_spec.h"
#include "picojson.h"
#include "triggers.h"

namespace openage {
namespace gameio {

picojson::value save_player(int player_number,openage::GameMain *game) {
	picojson::object player;
	player["civilisation"] = picojson::value(game->get_player(player_number)->civ->civ_name );
	player["player-id"]    = picojson::value( (double) player_number );
	player["color"]        = picojson::value( (double) game->get_player(player_number)->color );

	return picojson::value(player);
}

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
		properties["completion"] = picojson::value(unit->get_attribute<attr_type::building>().completed);
	} else {
		unitj["isbuilding"] = picojson::value(false);
	}
	unitj["properties"] = picojson::value(properties);
	return picojson::value(unitj);
}

void load_unit(picojson::object unit, openage::GameMain *game) {

	picojson::value unitv = picojson::value(unit);
	int pr_id	 = unitv.get("type").get<double>();
	int player_no	 = unitv.get("player").get<double>();
	coord::phys_t ne = unitv.get("position-ne").get<double>();
	coord::phys_t se = unitv.get("position-se").get<double>();

	picojson::object properties = unitv.get("properties").get<picojson::object>();

	// place unit on screen
	UnitType &saved_type = *game->get_player(player_no)->get_type(pr_id);
	auto ref = game->placed_units.new_unit(saved_type, game->players[player_no], coord::tile{ne, se}.to_phys2().to_phys3());

	// construct building
	bool has_building_attr = unitv.get("isbuilding").get<bool>();
	if (has_building_attr) {
		float completed = (float) picojson::value(properties).get("completion").get<double>();
		if (completed >= 1.0f && ref.is_valid()) {
			complete_building(*ref.get());
		}
	}
}

picojson::value save_tile_content( openage::TileContent *content) {
	picojson::object tile;
	tile["terrain-id"] = picojson::value((double) content->terrain_id);
	//TODO do we need this? see load_tile_content
	tile["size"]       = picojson::value((double) content->obj.size());
	return picojson::value(tile);
}
TileContent load_tile_content(picojson::object tile) {
	openage::TileContent content;
	content.terrain_id = picojson::value(tile).get("terrain-id").get<double>();

	//TODO do we need this? see save_tile_content
	//unsigned int o_size = picojson::value(tile).get("size").get<double>();
	return content;
}

Action* load_action_add_resource(picojson::value actionjson, game_resource resource) {
	float    amount = actionjson.get("amount").get<double>();
	uint16_t player = actionjson.get("player").get<double>();
	return new ActionAddResource(amount,player,resource);
}

Condition* load_condition_max_ressources(picojson::value conditionjson) {
	uint32_t player = conditionjson.get("player").get<double>();
	float value     = conditionjson.get("value") .get<double>();
	std::string res = conditionjson.get("resource").to_str();
	game_resource resource = game_resource::food;
	if( res.compare("gold") == 0) {
		resource = game_resource::gold;
	} else if( res.compare("wood") == 0) {
		resource = game_resource::wood;
	} else if( res.compare("stone") == 0) {
		resource = game_resource::stone;
	}
	return new ConditionMaxRessources(player, resource, value);
}

Condition* load_condition_min_ressources(picojson::value conditionjson) {
	uint32_t player = conditionjson.get("player").get<double>();
	float value     = conditionjson.get("value") .get<double>();
	std::string res = conditionjson.get("resource").to_str();
	game_resource resource = game_resource::food;
	if( res.compare("gold") == 0) {
		resource = game_resource::gold;
	} else if( res.compare("wood") == 0) {
		resource = game_resource::wood;
	} else if( res.compare("stone") == 0) {
		resource = game_resource::stone;
	}
	return new ConditionMinRessources(player, resource, value);
}

Trigger* load_trigger(picojson::object trigger, openage::GameMain *game) {
	log::log(MSG(dbg) << "loading trigger");
	Trigger *t = new Trigger();
	t->id = picojson::value(trigger).get("id").get<double>();
	t->isActivated = (bool) picojson::value(trigger).get("active").get<bool>();

	// gate
	std::string gate = picojson::value(trigger).get("gate").to_str();
	t->gate = Trigger::Gate::OR;

	if( gate.compare("and") == 0 ) {
		t->gate = Trigger::Gate::AND;
	} else if ( gate.compare("xor") == 0) {
		t->gate = Trigger::Gate::XOR;
	}

	// load actions
	picojson::array actions = picojson::value(trigger).get("actions").get<picojson::array>();
	Action* a;
	for (picojson::array::iterator iter = actions.begin(); iter != actions.end(); ++iter) {
		auto action = picojson::value((*iter).get<picojson::object>());
		if( action.get("type").to_str().compare("add-gold") == 0) {
			a = load_action_add_resource(action,game_resource::gold);
		} else if( action.get("type").to_str().compare("add-stone") == 0) {
			a = load_action_add_resource(action,game_resource::stone);
		} else if( action.get("type").to_str().compare("add-wood") == 0) {
			a = load_action_add_resource(action,game_resource::wood);
		} else if( action.get("type").to_str().compare("add-food") == 0) {
			a = load_action_add_resource(action,game_resource::food);
		}
		t->actions.push_back(a);
	}

	// load conditions
	picojson::array conditions = picojson::value(trigger).get("conditions").get<picojson::array>();
	Condition* c;
	for (picojson::array::iterator iter = conditions.begin(); iter != conditions.end(); ++iter) {
		auto condition = picojson::value((*iter).get<picojson::object>());
		std::string type = condition.get("type").to_str();
		if( type.compare("min-ressources") == 0) {
			c = load_condition_min_ressources(condition);
		} else if( type.compare("max-ressources") == 0) {
			c = load_condition_max_ressources(condition);
		}
		t->conditions.push_back(c);
	}
	return t;
}

void save(openage::GameMain *game, std::string fname) {
	log::log(MSG(dbg) << "saving " + fname);

	picojson::object savegame;
	//metadata
	savegame["label"] = picojson::value(save_label);
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
		chunkj["tile-count"]  = picojson::value((double) chunk->tile_count);

		// saving tiles
		picojson::array tiles;
		for (size_t p = 0; p < chunk->tile_count; ++p) {
			tiles.push_back( save_tile_content( chunk->get_data(p) ));
		}

		chunkj["tiles"] = picojson::value(tiles);
		chunks.push_back(picojson::value(chunkj));
	}
	terrain["chunks"]   = picojson::value(chunks);
	savegame["terrain"] = picojson::value(terrain);

	// save units
	std::vector<openage::Unit *> units = game->placed_units.all_units();
	picojson::array unitsj;
	for (Unit *u : units) {
		unitsj.push_back(save_unit(u));
	}
	savegame["units"] = picojson::value(unitsj);

	// save player info
	picojson::array players;
	int player_count = game->player_count();
	for(int i=0;i<player_count;i++) {
		players.push_back( save_player(i,game) );
	}
	savegame["players"] = picojson::value(players);

	// save to file
	std::ofstream file(fname, std::ofstream::out);
	file << picojson::value(savegame).serialize() << "\n";
}

void load(openage::GameMain *game, std::string fname, Engine *engine) {
	std::ifstream file(fname, std::ifstream::in);
	if (!file.good()) {
		log::log(MSG(dbg) << "could not find " + fname);
		return;
	}
	log::log(MSG(dbg) << "loading " + fname);

	//TODO json-schema check

	// to std::string
	std::istreambuf_iterator<char> first(file);
	std::istreambuf_iterator<char> last;
	std::string json_str(first, last);

	// parse json
	picojson::value savegame;
	std::string err;
	picojson::parse(savegame, json_str.begin(), json_str.end(), &err);

	//loading to json
	//picojson::object &savegame = json.get<picojson::object>();

	// load metadata
	std::string file_label = savegame.get("label").to_str();
	if (file_label != save_label) {
		log::log(MSG(warn) << fname << " is not a savefile");
		return;
	}
	std::string version = savegame.get("version").to_str();
	if (version != save_version) {
		log::log(MSG(warn) << "savefile has different version");
	}
	std::string build = savegame.get("build").to_str();

	// read terrain chunks
	//picojson::object &terrain = picojson::value(savegame).get("terrain").get<picojson::object>();
	picojson::object &terrain = savegame.get("terrain").get<picojson::object>();

	picojson::array chunks = picojson::value(terrain).get("chunks").get<picojson::array>();
	for (picojson::array::iterator iter = chunks.begin(); iter != chunks.end(); ++iter) {
		picojson::object &chunkjson = (*iter).get<picojson::object>();

		// chunk position
		coord::chunk_t ne = (int32_t) picojson::value(chunkjson).get("position-ne").get<double>();
		coord::chunk_t se = (int32_t) picojson::value(chunkjson).get("position-se").get<double>();
		openage::TerrainChunk *chunk = game->terrain->get_create_chunk(coord::chunk{ne, se});

		// tiles
		picojson::array tiles = picojson::value(chunkjson).get("tiles").get<picojson::array>();
		size_t p = 0;
		for (picojson::array::iterator iter = tiles.begin(); iter != tiles.end(); ++iter,++p) {
			*chunk->get_data(p) = load_tile_content( (*iter).get<picojson::object>() );
		}

	}

	// load units
	game->placed_units.reset();
	picojson::array units = picojson::value(savegame).get("units").get<picojson::array>();
	for (picojson::array::iterator iter = units.begin(); iter != units.end(); ++iter) {
		load_unit( (*iter).get<picojson::object>(), game );
	}

	// triggers

	Triggers* triggers = new Triggers ();
	engine->register_tick_action(triggers);
	if(!picojson::value(savegame).get("triggers").is<picojson::null>()) {
		picojson::array triggersj = picojson::value(savegame).get("triggers").get<picojson::array>();
		for (picojson::array::iterator iter = triggersj.begin(); iter != triggersj.end(); ++iter) {
			triggers->addTrigger( load_trigger( (*iter).get<picojson::object>(), game) );
		}
	}

	/*
	Condition *c0 = new ConditionMinRessources(1,game_resource::wood,5);
	Condition *c1 = new ConditionMaxRessources(1,game_resource::wood,500);
	ActionAddGold  *a0 = new ActionAddGold (1.0,1);
	ActionAddWood  *a1 = new ActionAddWood (2.0,1);
	ActionAddStone *a2 = new ActionAddStone(4.0,1);
	ActionAddFood  *a3 = new ActionAddFood (5.0,1);
	Trigger t;
	t.actions.push_back(a0);
	t.actions.push_back(a1);
	t.actions.push_back(a2);
	t.actions.push_back(a3);
	t.conditions.push_back(c0);
	t.conditions.push_back(c1);
	t.gate = Trigger::Trigger::Gate::AND;
	t.isActivated = true;
	t.isDeleted   = false;

	triggers->addTrigger(t);
	*/

}

}} // openage::gameio
