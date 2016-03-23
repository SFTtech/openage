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

void save(openage::GameMain *game, std::string fname) {
	log::log(MSG(dbg) << "saving " + fname);

	Json::Value savegame;

	//metadata
	savegame["label"]   = save_label;
	savegame["version"] = save_version;
	savegame["build"]   = config::version;


	// save terrain
	savegame["terrain"] = game->terrain->toJson();

	// save units
	Json::Value unitsj;
	for (Unit *u : game->placed_units.all_units()) {
		unitsj.append( u->toJson() );
	}
	savegame["units"] = unitsj;

	// save player info
	Json::Value players;
	int player_count = game->player_count();
	for(int i=0;i<player_count;i++) {
		players.append( game->get_player(i)->toJson() );
	}
	savegame["players"] = players;

	// save triggers
	Json::Value triggers;
	for(auto trigger : Triggers::getInstance()->getTriggers()) {
		triggers.append( trigger->toJson() );
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

	// load playera
	//game->players.clear();
	for (auto player : savegame["players"]) {
		//game->players.push_back( Player(player) );
		 load_player(player,game);
	}

	// read terrain chunks
	for (auto chunkjson : savegame["terrain"]["chunks"]) {
		game->terrain->get_create_chunk(chunkjson);
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
		triggers->addTrigger( new Trigger(trigger) );
	}
}

}} // openage::gameio
