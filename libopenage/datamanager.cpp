// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "unit/producer.h"
#include "util/strings.h"
#include "rng/global_rng.h"
#include "assetmanager.h"
#include "datamanager.h"
#include "engine.h"

namespace openage {

DataManager::DataManager()
	:
	gamedata_loaded{false} {}

DataManager::~DataManager() {}

void DataManager::initialize(AssetManager *am) {
	load_timer.start();

	this->assetmanager = am;
	auto gamedata_load_function = [this]() -> std::vector<gamedata::empiresdat> {
		log::log(MSG(info) << "loading game specification files... stand by, will be faster soon...");
		util::Dir gamedata_dir = this->assetmanager->get_data_dir()->append("converted/gamedata");
		return util::recurse_data_files<gamedata::empiresdat>(gamedata_dir, "gamedata-empiresdat.docx");
	};

	// add job
	Engine &engine = Engine::get();
	this->gamedata_load_job = engine.get_job_manager()->enqueue<std::vector<gamedata::empiresdat>>(gamedata_load_function);
}

void DataManager::check_updates() {
	if (not this->gamedata_loaded and this->gamedata_load_job.is_finished()) {
		auto gamedata = this->gamedata_load_job.get_result();
		this->on_gamedata_loaded(gamedata);
		this->gamedata_loaded = true;
		log::log(MSG(info).fmt("Loading time  [data]: %5.3f s", load_timer.getval() / 1e9));
	}
}

bool DataManager::load_complete() {
	return this->gamedata_loaded;
}

size_t DataManager::producer_count() {
	return this->available_objects.size();
}

index_t DataManager::get_slp_graphic(index_t slp) {
	return this->slp_to_graphic[slp];
}

Texture *DataManager::get_texture(index_t graphic_id) {
	if (graphic_id <= 0 || this->graphics.count(graphic_id) == 0) {
		log::log(DBG << "  -> ignoring graphics_id: " << graphic_id);
		return nullptr;
	}

	int slp_id = this->graphics[graphic_id]->slp_id;
	if (slp_id <= 0) {
		log::log(DBG << "  -> ignoring slp_id: " << slp_id);
		return nullptr;
	}

	log::log(DBG << "   slp id/name: " << slp_id << " " << this->graphics[graphic_id]->name0);
	std::string tex_fname = util::sformat("converted/graphics/%d.slp.png", slp_id);

	// get tex if file is available
	Texture *tex = nullptr;
	if (this->assetmanager->can_load(tex_fname)) {
		tex = this->assetmanager->get_texture(tex_fname);
	}
	return tex;
}

std::shared_ptr<UnitTexture> DataManager::get_unit_texture(index_t unit_id) {
	if (this->unit_textures.count(unit_id) == 0) {
		if (unit_id > 0) {
			log::log(DBG << "  -> ignoring unit_id: " << unit_id);
		}
		return nullptr;
	}
	return this->unit_textures[unit_id];
}

Sound *DataManager::get_sound(index_t sound_id) {
	if (this->available_sounds.count(sound_id) == 0) {
		if (sound_id > 0) {
			log::log(DBG << "  -> ignoring sound_id: " << sound_id);
		}
		return nullptr;
	}
	return &this->available_sounds[sound_id];
}

UnitType *DataManager::get_type(index_t type_id) {
	if (this->producers.count(type_id) == 0) {
		if (type_id > 0) {
			log::log(DBG << "  -> ignoring type_id: " << type_id);
		}
		return nullptr;
	}
	return this->producers[type_id];
}

UnitType *DataManager::get_type_index(size_t type_index) {
	if (type_index < available_objects.size()) {
		return available_objects[type_index].get();
	}
	log::log(DBG << "  -> ignoring type_index: " << type_index);
	return nullptr;
}

const gamedata::graphic *DataManager::get_graphic_data(index_t grp_id) {
	if (this->graphics.count(grp_id) == 0) {
		log::log(DBG << "  -> ignoring grp_id: " << grp_id);
		return nullptr;
	}
	return this->graphics[grp_id];
}

const gamedata::unit_building *DataManager::get_building_data(index_t unit_id) {
	if (this->buildings.count(unit_id) == 0) {
		log::log(DBG << "  -> ignoring unit_id: " << unit_id);
		return nullptr;
	}
	return this->buildings[unit_id];
}

std::vector<const gamedata::unit_command *> DataManager::get_command_data(index_t unit_id) {
	if (this->commands.count(unit_id) == 0) {
		return std::vector<const gamedata::unit_command *>(); // empty vector
	}
	return this->commands[unit_id];
}

void DataManager::on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata) {
	util::Dir *data_dir = this->assetmanager->get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");

	// create graphic id => graphic map
	for (auto &graphic : gamedata[0].graphics.data) {
		this->graphics[graphic.id] = &graphic;
		this->slp_to_graphic[graphic.slp_id] = graphic.id;
	}

	// create complete set of unit textures
	for (auto &g : this->graphics) {
		this->unit_textures.insert({g.first, std::make_shared<UnitTexture>(this, g.second)});
	}

	auto get_sound_file_location = [asset_dir](int32_t resource_id) -> std::string {
		std::string snd_file_location = util::sformat("sounds/%d.opus", resource_id);
		if (util::file_size(asset_dir.join(snd_file_location)) > 0) {
			return snd_file_location;
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
				log::log(SPAM <<
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
	am.load_resources(asset_dir, sound_files);

	// this final step occurs after loading media
	// as producers require both the graphics and sounds

	int your_civ_id = 1; //British by default
	// 0 is gaia and not very useful (it's not an user facing civilization so
	// we cannot rely on it being polished... it might be VERY broken.
	// British or any other civ is a way safer bet.

	log::log(MSG(info) << "Using the " << gamedata[0].civs.data[your_civ_id].name << " civilisation.");
	this->create_abilities(gamedata);
	this->create_unit_types(gamedata, your_civ_id);
}

bool DataManager::valid_graphic_id(index_t graphic_id) {
	if (graphic_id <= 0 || this->graphics.count(graphic_id) == 0) {
		return false;
	}
	if (this->graphics[graphic_id]->slp_id <= 0) {
		return false;
	}
	return true;
}

void DataManager::load_building(const gamedata::unit_building &building, unit_type_list &list) {

	// check graphics
	if (this->valid_graphic_id(building.graphic_standing0)) {
		list.emplace_back(std::make_unique<BuildingProducer>(*this, &building));

		auto producer_ptr = list.back().get();
		this->producers[producer_ptr->id()] = producer_ptr;
		this->buildings[building.id0] = &building;
	}
}

void DataManager::load_living(const gamedata::unit_living &unit, unit_type_list &list) {

	// check graphics
	if (this->valid_graphic_id(unit.graphic_dying0) &&
		this->valid_graphic_id(unit.graphic_standing0) &&
		this->valid_graphic_id(unit.walking_graphics0)) {
		list.emplace_back(std::make_unique<LivingProducer>(*this, &unit));

		auto producer_ptr = list.back().get();
		this->producers[producer_ptr->id()] = producer_ptr;
	}
}

void DataManager::load_object(const gamedata::unit_object &object, unit_type_list &list) {

	// check graphics
	if (this->valid_graphic_id(object.graphic_standing0)) {
		list.emplace_back(std::make_unique<ObjectProducer>(*this, &object));

		auto producer_ptr = list.back().get();
		this->producers[producer_ptr->id()] = producer_ptr;
	}
}

void DataManager::load_projectile(const gamedata::unit_projectile &proj, unit_type_list &list) {

	// check graphics
	if (this->valid_graphic_id(proj.graphic_standing0)) {
		list.emplace_back(std::make_unique<ProjectileProducer>(*this, &proj));

		auto producer_ptr = list.back().get();
		this->producers[producer_ptr->id()] = producer_ptr;
	}
}

void DataManager::create_abilities(const std::vector<gamedata::empiresdat> &gamedata) {
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

void DataManager::create_unit_types(const std::vector<gamedata::empiresdat> &gamedata, int your_civ_id) {

	// create projectile types first
	for (auto &obj : gamedata[0].civs.data[0].units.projectile.data) {
		this->load_projectile(obj, available_objects);
	}

	// create object unit types
	for (auto &obj : gamedata[0].civs.data[0].units.object.data) {
		this->load_object(obj, available_objects);
	}

	// create dead unit types
	for (auto &unit : gamedata[0].civs.data[0].units.dead_or_fish.data) {
		this->load_object(unit, available_objects);
	}

	// create living unit types
	for (auto &unit : gamedata[0].civs.data[your_civ_id].units.living.data) {
		this->load_living(unit, available_objects);
	}

	// create building unit types
	for (auto &building : gamedata[0].civs.data[your_civ_id].units.building.data) {
		this->load_building(building, available_objects);
	}
}

void Sound::play() {
	if (this->sound_items.size() <= 0) {
		return;
	}
	audio::AudioManager &am = Engine::get().get_audio_manager();

	int rand = rng::random_range(0, this->sound_items.size());
	int sndid = this->sound_items[rand];
	try {
		audio::Sound{am.get_sound(audio::category_t::GAME, sndid)}.play();
	}
	catch (Error &e) {
		log::log(MSG(warn) << "cannot play: " << e);
	}
}

} // openage
