// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "game_save.h"

#include <fstream>
#include <vector>
#include <string>

#include <json/json.h>

#include "../engine.h"
#include "../log/log.h"
#include "../unit/producer.h"
#include "../unit/unit.h"
#include "../unit/unit_type.h"
#include "game_main.h"
#include "game_save.h"
#include "game_spec.h"
#include "trigger.h"
#include "triggers.h"

namespace openage {
namespace gameio {

Json::Value save_player(int player_number,openage::GameMain *game) {

	Json::Value player;

	player["civilisation"] = game->get_player(player_number)->civ->civ_name;
	player["player-id"]    = player_number;
	player["color"]        = game->get_player(player_number)->color;

	player["gold"]         = (double) game->get_player(player_number)->amount(game_resource::gold);
	player["stone"]        = (double) game->get_player(player_number)->amount(game_resource::stone);
	player["wood"]         = (double) game->get_player(player_number)->amount(game_resource::wood);
	player["food"]         = (double) game->get_player(player_number)->amount(game_resource::food);

	return player;
}

void load_player(Json::Value player,openage::GameMain *game) {

	uint16_t id= player.get("player-id",0).asInt();

	double gold = player.get("gold" ,0).asDouble();
	double stone= player.get("stone",0).asDouble();
	double wood = player.get("wood" ,0).asDouble();
	double food = player.get("food" ,0).asDouble();

	game->get_player(id)->receive(game_resource::gold ,gold);
	game->get_player(id)->receive(game_resource::stone,stone);
	game->get_player(id)->receive(game_resource::wood ,wood);
	game->get_player(id)->receive(game_resource::food ,food);
}

Json::Value save_unit(Unit *unit) {
	Json::Value unitj;
	unitj["type"]   =  (double) unit->unit_type->id();
	unitj["player"] =  (double) unit->get_attribute<attr_type::owner>().player.player_number;

	//position
	coord::tile pos = unit->location->pos.start;
	unitj["position-ne"] =  (double) pos.ne;
	unitj["position-se"] =  (double) pos.se;

	//unit properties
	Json::Value properties;
	bool has_building_attr = unit->has_attribute(attr_type::building);
	if (has_building_attr) {
		unitj["isbuilding"] = true;
		properties["completion"] = unit->get_attribute<attr_type::building>().completed;
	} else {
		unitj["isbuilding"] = false;
	}
	unitj["properties"] = properties;
	return unitj;
}

void load_unit(Json::Value unit, openage::GameMain *game) {

	int pr_id	 = unit.get("type",0).asInt();
	int player_no	 = unit.get("player",0).asInt();
	coord::phys_t ne = unit.get("position-ne",0).asInt64();
	coord::phys_t se = unit.get("position-se",0).asInt64();

	Json::Value properties = unit["properties"];

	// place unit on screen
	UnitType &saved_type = *game->get_player(player_no)->get_type(pr_id);
	auto ref = game->placed_units.new_unit(saved_type, game->players[player_no], coord::tile{ne, se}.to_phys2().to_phys3());

	// construct building
	bool has_building_attr = unit.get("isbuilding",false).asBool();
	if (has_building_attr) {
		float completed = properties.get("completion",0).asFloat();
		if (completed >= 1.0f && ref.is_valid()) {
			complete_building(*ref.get());
		}
	}
}

Json::Value save_tile_content(openage::TileContent *content) {
	Json::Value tile;
	tile["terrain-id"] = (double) content->terrain_id;
	//TODO do we need this? see load_tile_content
	tile["size"]       = (double) content->obj.size();
	return tile;
}
TileContent load_tile_content(Json::Value tile) {
	openage::TileContent content;
	content.terrain_id = tile.get("terrain-id",0).asInt();

	//TODO do we need this? see save_tile_content
	//unsigned int o_size = picojson::value(tile).get("size").get<double>();
	return content;
}

Action* load_action_add_resource(Json::Value actionjson, game_resource resource) {
	float    amount = actionjson.get("amount",0).asFloat();
	uint16_t player = actionjson.get("player",0).asUInt();
	return new ActionAddResource(amount,player,resource);
}

Condition* load_condition_every_tick(Json::Value conditionjson) {
	return new ConditionEveryTick();
}

Condition* load_condition_timer_loop(Json::Value conditionjson) {
	uint16_t ms = conditionjson.get("value",0).asUInt();
	return new ConditionTimerLoop(ms);
}

Condition* load_condition_max_ressources(Json::Value conditionjson) {
	uint32_t player = conditionjson.get("player",0).asUInt();
	float value     = conditionjson.get("value",0) .asFloat();
	std::string res = conditionjson.get("resource","food").asString();
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

Condition* load_condition_min_ressources(Json::Value conditionjson) {
	uint32_t player = conditionjson.get("player",0).asUInt();
	float value     = conditionjson.get("value",0) .asFloat();
	std::string res = conditionjson.get("resource","food").asString();
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

Json::Value save_trigger(Trigger *trigger) {
	Json::Value triggerj;

	triggerj["id"]      = trigger->id;
	triggerj["gate"]    = trigger->getGateString();
	triggerj["active"]  = trigger->isActivated;
	triggerj["deleted"] = trigger->isDeleted;

	// save actions
	Json::Value actions;
	for(auto action : trigger->actions) {
		actions.append( action->toJson() );
	}
	// free triggers
	triggerj["actions"] = actions;

	// save conditions
	Json::Value conditions;
	for(auto condition : trigger->conditions) {
		conditions.append( condition->toJson() );
	}
	triggerj["conditions"] = conditions;

	return triggerj;
}

Trigger* load_trigger(Json::Value trigger, openage::GameMain *game) {
	log::log(MSG(dbg) << "loading trigger");
	Trigger *t = new Trigger();
	t->id = trigger.get("id",0).asInt64();
	t->isActivated = trigger.get("active",true).asBool();

	// gate
	std::string gate = trigger.get("gate","or").asString();
	t->gate = Trigger::Gate::OR;

	if( gate.compare("and") == 0 ) {
		t->gate = Trigger::Gate::AND;
	} else if ( gate.compare("xor") == 0) {
		t->gate = Trigger::Gate::XOR;
	}

	// load actions
	Action* a;
	for (auto action : trigger["actions"]) {
		std::string type = action.get("type","unkown").asString();
		if( type.compare("add-gold") == 0) {
			a = load_action_add_resource(action,game_resource::gold);
		} else if( type.compare("add-stone") == 0) {
			a = load_action_add_resource(action,game_resource::stone);
		} else if( type.compare("add-wood") == 0) {
			a = load_action_add_resource(action,game_resource::wood);
		} else if( type.compare("add-food") == 0) {
			a = load_action_add_resource(action,game_resource::food);
		}
		if(type.compare("unkown") != 0) {
			t->actions.push_back(a);
		}
	}

	// load conditions
	Condition* c;
	for (auto condition : trigger["conditions"]) {
		std::string type = condition.get("type","unkown").asString();
		if( type.compare("min-resources") == 0) {
			c = load_condition_min_ressources(condition);
		} else if( type.compare("max-resources") == 0) {
			c = load_condition_max_ressources(condition);
		} else if( type.compare("timer-loop") == 0) {
			c = load_condition_timer_loop(condition);
		} else if( type.compare("every-tick") == 0) {
			c = load_condition_every_tick(condition);
		}
		if(type.compare("unkown") != 0) {
			t->conditions.push_back(c);
		}
	}
	return t;
}

void save(openage::GameMain *game, std::string fname) {
	log::log(MSG(dbg) << "saving " + fname);

	Json::Value savegame;

	//picojson::object savegame;
	//metadata
	savegame["label"]   = save_label;
	savegame["version"] = save_version;
	savegame["build"]   = config::version;

	// saving terrain
	Json::Value terrain;
	// how many chunks
	std::vector<coord::chunk> used = game->terrain->used_chunks();
	terrain["chunks-size"] = (double) used.size();
	Json::Value chunks;

	// loop through chunks
	for (coord::chunk &position : used) {
		openage::TerrainChunk *chunk = game->terrain->get_chunk(position);
		Json::Value chunkj;

		//chunk metadata
		chunkj["position-se"] = position.se;
		chunkj["position-ne"] = position.ne;
		chunkj["tile-count"]  = (double) chunk->tile_count;

		// saving tiles
		Json::Value tiles;
		for (size_t p = 0; p < chunk->tile_count; ++p) {
			tiles.append( save_tile_content( chunk->get_data(p) ));
		}

		chunkj["tiles"] = tiles;
		chunks.append( chunkj);
	}
	terrain["chunks"]   = chunks;
	savegame["terrain"] = terrain;

	// save units
	std::vector<openage::Unit *> units = game->placed_units.all_units();
	Json::Value unitsj;
	for (Unit *u : units) {
		unitsj.append(save_unit(u));
	}
	savegame["units"] = unitsj;

	// save player info
	Json::Value players;
	int player_count = game->player_count();
	for(int i=0;i<player_count;i++) {
		players.append( save_player(i,game) );
	}
	savegame["players"] = players;

	// save triggers
	Json::Value triggers;
	for(auto trigger : Triggers::getInstance()->getTriggers()) {
		triggers.append( save_trigger(trigger) );
	}

	// free triggers
	Triggers::getInstance()->reset();
	savegame["triggers"] = triggers;

	// save to file
	Json::StyledWriter styledWriter;
	std::ofstream file(fname, std::ofstream::out);
	file << styledWriter.write(savegame);
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

	Json::Value savegame;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( json_str, savegame );
	if ( !parsingSuccessful ) {
	    log::log(MSG(dbg) << "savegame is not a valid json file (" + fname + "()");
	    return;
	}

	// load metadata
	std::string file_label = savegame.get("label","unkown").asString();
	if (file_label != save_label) {
		log::log(MSG(warn) << fname << " is not a savefile");
		return;
	}
	std::string version = savegame.get("version","unkown").asString();
	if (version != save_version) {
		log::log(MSG(warn) << "savefile has different version");
	}
	std::string build = savegame.get("build","unkown").asString();

	// read terrain chunks
	for (auto chunkjson : savegame["terrain"]["chunks"]) {

		// chunk position
		coord::chunk_t ne = chunkjson.get("position-ne",0).asInt();
		coord::chunk_t se = chunkjson.get("position-se",0).asInt();
		openage::TerrainChunk *chunk = game->terrain->get_create_chunk(coord::chunk{ne, se});

		// tiles
		size_t p = 0;
		for (auto tile : chunkjson["tiles"]) {
			*chunk->get_data(p) = load_tile_content( tile );
		}

	}

	// load player
	for (auto player : savegame["players"]) {
		load_player( player, game );
	}

	// load units
	game->placed_units.reset();
	for (auto unit : savegame["units"]) {
		load_unit( unit, game );
	}

	// triggers
	Triggers* triggers = Triggers::getInstance();
	triggers->reset();
	engine->register_tick_action(triggers);
	for (auto trigger : savegame["triggers"]) {
		triggers->addTrigger( load_trigger( trigger, game) );
	}
}

}} // openage::gameio
