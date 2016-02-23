// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../gamedata/blending_mode.gen.h"
#include "../gamedata/string_resource.gen.h"
#include "../gamedata/terrain.gen.h"
#include "../unit/producer.h"
#include "../util/strings.h"
#include "../rng/global_rng.h"
#include "../assetmanager.h"
#include "../engine.h"
#include "civilisation.h"
#include "game_spec.h"

namespace openage {

GameSpec::GameSpec(AssetManager &am)
	:
	data_path{"converted/gamedata"},
	graphics_path{"converted/graphics"},
	terrain_path{"converted/terrain"},
	blend_path{"converted/blendomatic"},
	sound_path{"converted/sounds/"},
	gamedata_loaded{false} {

	// begin loading gamedata
	this->initialize(am);
}

GameSpec::~GameSpec() {}

void GameSpec::initialize(AssetManager &am) {
	load_timer.start();

	this->load_terrain(am);

	this->assetmanager = &am;
	auto gamedata_load_function = [this]() {
		log::log(MSG(info) << "loading game specification files... stand by, will be faster soon...");
		util::Dir gamedata_dir = this->assetmanager->get_data_dir()->append(this->data_path);
		this->gamedata = util::recurse_data_files<gamedata::empiresdat>(gamedata_dir, "gamedata-empiresdat.docx");
		this->on_gamedata_loaded(this->gamedata);
		this->gamedata_loaded = true;
		log::log(MSG(info).fmt("Loading time  [data]: %5.3f s", load_timer.getval() / 1e9));
		return true;
	};

	// add job
	Engine &engine = Engine::get();
	this->gamedata_load_job = engine.get_job_manager()->enqueue<bool>(gamedata_load_function);
}

bool GameSpec::load_complete() const {
	return this->gamedata_loaded;
}

terrain_meta *GameSpec::get_terrain_meta() {
	return &this->terrain_data;
}

index_t GameSpec::get_slp_graphic(index_t slp) {
	return this->slp_to_graphic[slp];
}

Texture *GameSpec::get_texture(index_t graphic_id) const {
	if (graphic_id <= 0 || this->graphics.count(graphic_id) == 0) {
		log::log(MSG(dbg) << "  -> ignoring graphics_id: " << graphic_id);
		return nullptr;
	}

	auto g = this->graphics.at(graphic_id);
	int slp_id = g->slp_id;
	if (slp_id <= 0) {
		log::log(MSG(dbg) << "  -> ignoring slp_id: " << slp_id);
		return nullptr;
	}

	log::log(MSG(dbg) << "   slp id/name: " << slp_id << " " << g->name0);
	std::string tex_fname = util::sformat("%s/%d.slp.png", this->graphics_path.c_str(), slp_id);

	// get tex if file is available
	Texture *tex = nullptr;
	if (this->assetmanager->can_load(tex_fname)) {
		tex = this->assetmanager->get_texture(tex_fname);
	}
	return tex;
}

std::shared_ptr<UnitTexture> GameSpec::get_unit_texture(index_t unit_id) const {
	if (this->unit_textures.count(unit_id) == 0) {
		if (unit_id > 0) {
			log::log(MSG(dbg) << "  -> ignoring unit_id: " << unit_id);
		}
		return nullptr;
	}
	return this->unit_textures.at(unit_id);
}

const Sound *GameSpec::get_sound(index_t sound_id) const {
	if (this->available_sounds.count(sound_id) == 0) {
		if (sound_id > 0) {
			log::log(MSG(dbg) << "  -> ignoring sound_id: " << sound_id);
		}
		return nullptr;
	}
	return &this->available_sounds.at(sound_id);
}


const gamedata::graphic *GameSpec::get_graphic_data(index_t grp_id) const {
	if (this->graphics.count(grp_id) == 0) {
		log::log(MSG(dbg) << "  -> ignoring grp_id: " << grp_id);
		return nullptr;
	}
	return this->graphics.at(grp_id);
}

std::vector<const gamedata::unit_command *> GameSpec::get_command_data(index_t unit_id) const {
	if (this->commands.count(unit_id) == 0) {
		return std::vector<const gamedata::unit_command *>(); // empty vector
	}
	return this->commands.at(unit_id);
}

std::string GameSpec::get_civ_name(int civ_id) const {
	return gamedata[0].civs.data[civ_id].name;
}

void GameSpec::create_unit_types(unit_meta_list &objects, int civ_id) const {
	if (!this->load_complete()) {
		return;
	}

	// create projectile types first
	for (auto &obj : gamedata[0].civs.data[civ_id].units.projectile.data) {
		this->load_projectile(obj, objects);
	}

	// create object unit types
	for (auto &obj : gamedata[0].civs.data[civ_id].units.object.data) {
		this->load_object(obj, objects);
	}

	// create dead unit types
	for (auto &unit : gamedata[0].civs.data[civ_id].units.dead_or_fish.data) {
		this->load_object(unit, objects);
	}

	// create living unit types
	for (auto &unit : gamedata[0].civs.data[civ_id].units.living.data) {
		this->load_living(unit, objects);
	}

	// create building unit types
	for (auto &building : gamedata[0].civs.data[civ_id].units.building.data) {
		this->load_building(building, objects);
	}
}

void GameSpec::on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata) {
	util::Dir *data_dir = this->assetmanager->get_data_dir();
	util::Dir sound_dir = data_dir->append(this->sound_path);

	// create graphic id => graphic map
	for (auto &graphic : gamedata[0].graphics.data) {
		this->graphics[graphic.id] = &graphic;
		this->slp_to_graphic[graphic.slp_id] = graphic.id;
	}

	// create complete set of unit textures
	for (auto &g : this->graphics) {
		this->unit_textures.insert({g.first, std::make_shared<UnitTexture>(*this, g.second)});
	}

	auto get_sound_file_location = [sound_dir](int32_t resource_id) -> std::string {
		std::string snd_filename = util::sformat("/%d.opus", resource_id);
		if (util::file_size(sound_dir.join(snd_filename)) > 0) {
			return snd_filename;
		}

		// We could not find the sound file for the provided resource_id in both directories
		return "";
	};

	// playable sound files for the audio manager
	std::vector<gamedata::sound_file> sound_files;
	for (gamedata::sound &sound : gamedata[0].sounds.data) {
		std::vector<int> sound_items;

		for (gamedata::sound_item &item : sound.sound_items.data) {
			std::string snd_file_location = get_sound_file_location(item.resource_id);
			if (snd_file_location.empty()) {
				log::log(MSG(warn) <<
					"   No sound file found for resource_id " <<
					item.resource_id << ", ignoring...");
				continue;
			}

			sound_items.push_back(item.resource_id);

			gamedata::sound_file f {
				gamedata::audio_category_t::GAME,
				item.resource_id,
				snd_file_location,
				gamedata::audio_format_t::OPUS,
				gamedata::audio_loader_policy_t::DYNAMIC
			};
			sound_files.push_back(f);
		}
		// create test sound objects that can be played later
		this->available_sounds[sound.id] = Sound{sound_items};
	}

	// load the requested sounds.
	Engine &engine = Engine::get();
	audio::AudioManager &am = engine.get_audio_manager();
	am.load_resources(sound_dir, sound_files);

	// this final step occurs after loading media
	// as producers require both the graphics and sounds
	this->create_abilities(gamedata);
}

bool GameSpec::valid_graphic_id(index_t graphic_id) const {
	if (graphic_id <= 0 || this->graphics.count(graphic_id) == 0) {
		return false;
	}
	if (this->graphics.at(graphic_id)->slp_id <= 0) {
		return false;
	}
	return true;
}

void GameSpec::load_building(const gamedata::unit_building &building, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(building.graphic_standing0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Building", building.id0, [this, &building](const Player &owner) {
			return std::make_shared<BuildingProducer>(owner, *this, &building);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_living(const gamedata::unit_living &unit, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(unit.graphic_dying0) &&
		this->valid_graphic_id(unit.graphic_standing0) &&
		this->valid_graphic_id(unit.walking_graphics0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Living", unit.id0, [this, &unit](const Player &owner) {
			return std::make_shared<LivingProducer>(owner, *this, &unit);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_object(const gamedata::unit_object &object, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(object.graphic_standing0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Object", object.id0, [this, &object](const Player &owner) {
			return std::make_shared<ObjectProducer>(owner, *this, &object);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_projectile(const gamedata::unit_projectile &proj, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(proj.graphic_standing0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Projectile", proj.id0, [this, &proj](const Player &owner) {
			return std::make_shared<ProjectileProducer>(owner, *this, &proj);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_terrain(AssetManager &am) {

	// Terrain data files
	util::Dir *data_dir = am.get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");
	std::vector<gamedata::string_resource> string_resources;
	util::read_csv_file(asset_dir.join("string_resources.docx"), string_resources);
	std::vector<gamedata::terrain_type> terrain_meta;
	util::read_csv_file(asset_dir.join("gamedata/gamedata-empiresdat/0000-terrains.docx"), terrain_meta);
	std::vector<gamedata::blending_mode> blending_meta;
	util::read_csv_file(asset_dir.join("blending_modes.docx"), blending_meta);

	// remove any disabled textures
	terrain_meta.erase(
		std::remove_if(terrain_meta.begin(), terrain_meta.end(),
			[](const gamedata::terrain_type &t) { return !t.enabled; }),
		terrain_meta.end());

	// result attributes
	terrain_data.terrain_id_count         = terrain_meta.size();
	terrain_data.blendmode_count          = blending_meta.size();
	terrain_data.textures.reserve(terrain_data.terrain_id_count);
	terrain_data.blending_masks.reserve(terrain_data.blendmode_count);
	terrain_data.terrain_id_priority_map  = std::make_unique<int[]>(terrain_data.terrain_id_count);
	terrain_data.terrain_id_blendmode_map = std::make_unique<int[]>(terrain_data.terrain_id_count);
	terrain_data.influences_buf           = std::make_unique<struct influence[]>(terrain_data.terrain_id_count);


	log::log(MSG(dbg) << "Terrain prefs: " <<
		"tiletypes=" << terrain_data.terrain_id_count << ", "
		"blendmodes=" << terrain_data.blendmode_count);

	// create tile textures (snow, ice, grass, whatever)
	for (size_t i = 0; i < terrain_data.terrain_id_count; i++) {
		auto line = &terrain_meta[i];
		terrain_t terrain_id = i;

		// TODO: validate terrain_id < terrain_id_count

		// TODO: terrain double-define check?
		terrain_data.terrain_id_priority_map[terrain_id]  = line->blend_priority;
		terrain_data.terrain_id_blendmode_map[terrain_id] = line->blend_mode;

		// TODO: remove hardcoding and rely on nyan data
		auto terraintex_filename = util::sformat("%s/%d.slp.png", this->terrain_path.c_str(), line->slp_id);
		auto new_texture = am.get_texture(terraintex_filename);

		terrain_data.textures[terrain_id] = new_texture;
	}

	// create blending masks (see doc/media/blendomatic)
	for (size_t i = 0; i < terrain_data.blendmode_count; i++) {
		auto line = &blending_meta[i];

		std::string mask_filename = util::sformat("%s/mode%02d.png", this->blend_path.c_str(), line->blend_mode);
		terrain_data.blending_masks[i] = am.get_texture(mask_filename);
	}
}

void GameSpec::create_abilities(const std::vector<gamedata::empiresdat> &gamedata) {
	// use game data unit commands
	int headers =  gamedata[0].unit_headers.data.size();
	int total = 0;

	// it seems the index of the header indicates the unit
	for (int i = 0; i < headers; ++i) {

		// init vector
		std::vector<const gamedata::unit_command *> list;

		// add each element
		auto &head = gamedata[0].unit_headers.data[i];
		for (auto &cmd : head.unit_commands.data) {
			total++;

			// commands either have a class id or a unit id
			// log::dbg("unit command %d %d -> class %d, unit %d, resource %d", i, cmd.id, cmd.class_id, cmd.unit_id, cmd.resource_in);
			list.push_back(&cmd);
		}

		//insert to map
		this->commands[i] = list;
	}
}

void Sound::play() const {
	if (this->sound_items.size() <= 0) {
		return;
	}
	audio::AudioManager &am = Engine::get().get_audio_manager();

	int rand = rng::random_range(0, this->sound_items.size());
	int sndid = this->sound_items.at(rand);
	try {
		audio::Sound{am.get_sound(audio::category_t::GAME, sndid)}.play();
	}
	catch(Error &e) {
		log::log(MSG(warn) << "cannot play: " << e);
	}
}

}
