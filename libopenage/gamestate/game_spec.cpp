// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "game_spec.h"

#include <tuple>

#include "../assetmanager.h"
#include "../audio/error.h"
#include "../audio/resource_def.h"
#include "../engine.h"
#include "../gamedata/blending_mode.gen.h"
#include "../gamedata/string_resource.gen.h"
#include "../gamedata/terrain.gen.h"
#include "../log/log.h"
#include "../rng/global_rng.h"
#include "../unit/producer.h"
#include "../util/compiler.h"
#include "../util/strings.h"
#include "../util/timer.h"
#include "civilisation.h"


namespace openage {

GameSpec::GameSpec(AssetManager *am)
	:
	assetmanager{am},
	gamedata_loaded{false} {
}

GameSpec::~GameSpec() = default;


bool GameSpec::initialize() {
	util::Timer load_timer;
	load_timer.start();

	const util::Path &asset_dir = this->assetmanager->get_asset_dir();

	log::log(MSG(info) << "Loading game specification files...");

	std::vector<gamedata::string_resource> string_resources = util::read_csv_file<gamedata::string_resource>(
		asset_dir["converted/string_resources.docx"]
	);

	try {
		// read the packed csv file
		util::CSVCollection raw_gamedata{
			asset_dir["converted/gamedata/gamedata.docx"]
		};

		// parse the original game description files
		this->gamedata = raw_gamedata.read<gamedata::empiresdat>(
			"gamedata-empiresdat.docx"
		);

		this->load_terrain(this->gamedata[0]);

		// process and load the game description files
		this->on_gamedata_loaded(this->gamedata[0]);
		this->gamedata_loaded = true;
	}
	catch (Error &exc) {
		// rethrow allmighty openage exceptions
		throw;
	}
	catch (std::exception &exc) {
		// unfortunately we have no idea of the std::exception backtrace
		throw Error{ERR << "gamedata could not be loaded: "
		                << util::demangle(typeid(exc).name())
		                << ": "<< exc.what()};
	}

	log::log(MSG(info).fmt("Loading time  [data]: %5.3f s",
	                       load_timer.getval() / 1e9));
	return true;
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
		log::log(MSG(dbg) << "  -> ignoring negative slp_id: " << slp_id);
		return nullptr;
	}

	log::log(MSG(dbg) << "   slp id/name: " << slp_id << " " << g->name);
	std::string tex_fname = util::sformat("converted/graphics/%d.slp.png", slp_id);

	return this->get_texture(tex_fname, true);
}

Texture *GameSpec::get_texture(const std::string &file_name, bool use_metafile) const {
	// return nullptr if the texture wasn't found (3rd param)
	return this->assetmanager->get_texture(file_name, use_metafile, true);
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
	return this->gamedata[0].civs.data[civ_id].name;
}

void GameSpec::create_unit_types(unit_meta_list &objects, int civ_id) const {
	if (!this->load_complete()) {
		return;
	}

	// create projectile types first
	for (auto &obj : this->gamedata[0].civs.data[civ_id].units.missile.data) {
		this->load_missile(obj, objects);
	}

	// create object unit types
	for (auto &obj : this->gamedata[0].civs.data[civ_id].units.object.data) {
		this->load_object(obj, objects);
	}

	// create dead unit types
	for (auto &unit : this->gamedata[0].civs.data[civ_id].units.moving.data) {
		this->load_object(unit, objects);
	}

	// create living unit types
	for (auto &unit : this->gamedata[0].civs.data[civ_id].units.living.data) {
		this->load_living(unit, objects);
	}

	// create building unit types
	for (auto &building : this->gamedata[0].civs.data[civ_id].units.building.data) {
		this->load_building(building, objects);
	}
}


AssetManager *GameSpec::get_asset_manager() const {
	return this->assetmanager;
}


void GameSpec::on_gamedata_loaded(const gamedata::empiresdat &gamedata) {
	const util::Path &asset_dir = this->assetmanager->get_asset_dir();
	util::Path sound_dir = asset_dir["converted/sounds"];

	// create graphic id => graphic map
	for (auto &graphic : gamedata.graphics.data) {
		this->graphics[graphic.graphic_id] = &graphic;
		this->slp_to_graphic[graphic.slp_id] = graphic.graphic_id;
	}

	log::log(INFO << "Loading textures...");

	// create complete set of unit textures
	for (auto &g : this->graphics) {
		this->unit_textures.insert({g.first, std::make_shared<UnitTexture>(*this, g.second)});
	}

	log::log(INFO << "Loading sounds...");

	// playable sound files for the audio manager
	std::vector<audio::resource_def> load_sound_files;

	// all sounds defined in the game specification
	for (const gamedata::sound &sound : gamedata.sounds.data) {
		std::vector<int> sound_items;

		// each sound may have multiple variation,
		// processed in this loop
		// these are the single sound files.
		for (const gamedata::sound_item &item : sound.sound_items.data) {

			if (item.resource_id < 0) {
				log::log(SPAM << "   Invalid sound resource id < 0");
				continue;
			}

			std::string snd_filename = util::sformat("%d.opus", item.resource_id);
			util::Path snd_path = sound_dir[snd_filename];

			if (not snd_path.is_file()) {
				continue;
			}

			// single items for a sound (so that we can ramdomize it)
			sound_items.push_back(item.resource_id);

			// the single sound will be loaded in the audio system.
			audio::resource_def resource {
				audio::category_t::GAME,
				item.resource_id,
				snd_path,
				audio::format_t::OPUS,
				audio::loader_policy_t::DYNAMIC
			};
			load_sound_files.push_back(resource);
		}


		// create test sound objects that can be played later
		this->available_sounds.insert({
			sound.sound_id,
			Sound{
				this,
				std::move(sound_items)
			}
		});
	}

	// TODO: move out the loading of the sound.
	//       this class only provides the names and locations

	// load the requested sounds.
	audio::AudioManager &am = this->assetmanager->get_engine()->get_audio_manager();
	am.load_resources(load_sound_files);

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

void GameSpec::load_building(const gamedata::building_unit &building, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(building.idle_graphic0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Building", building.id0, [this, &building](const Player &owner) {
			return std::make_shared<BuildingProducer>(owner, *this, &building);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_living(const gamedata::living_unit &unit, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(unit.dying_graphic) &&
		this->valid_graphic_id(unit.idle_graphic0) &&
		this->valid_graphic_id(unit.walking_graphics0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Living", unit.id0, [this, &unit](const Player &owner) {
			return std::make_shared<LivingProducer>(owner, *this, &unit);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_object(const gamedata::unit_object &object, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(object.idle_graphic0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Object", object.id0, [this, &object](const Player &owner) {
			return std::make_shared<ObjectProducer>(owner, *this, &object);
		});
		list.emplace_back(meta_type);
	}
}

void GameSpec::load_missile(const gamedata::missile_unit &proj, unit_meta_list &list) const {

	// check graphics
	if (this->valid_graphic_id(proj.idle_graphic0)) {
		auto meta_type = std::make_shared<UnitTypeMeta>("Projectile", proj.id0, [this, &proj](const Player &owner) {
			return std::make_shared<ProjectileProducer>(owner, *this, &proj);
		});
		list.emplace_back(meta_type);
	}
}


void GameSpec::load_terrain(const gamedata::empiresdat &gamedata) {

	// fetch blending modes
	util::Path convert_dir = this->assetmanager->get_asset_dir()["converted"];
	std::vector<gamedata::blending_mode> blending_meta = util::read_csv_file<gamedata::blending_mode>(
		convert_dir["blending_modes.docx"]
	);

	// copy the terrain metainformation
	std::vector<gamedata::terrain_type> terrain_meta = gamedata.terrains.data;

	// remove any disabled textures
	terrain_meta.erase(
		std::remove_if(
			terrain_meta.begin(),
			terrain_meta.end(),
			[] (const gamedata::terrain_type &t) {
				return not t.enabled;
			}
		),
		terrain_meta.end()
	);

	// result attributes
	this->terrain_data.terrain_id_count         = terrain_meta.size();
	this->terrain_data.blendmode_count          = blending_meta.size();
	this->terrain_data.textures.resize(terrain_data.terrain_id_count);
	this->terrain_data.blending_masks.reserve(terrain_data.blendmode_count);
	this->terrain_data.terrain_id_priority_map  = std::make_unique<int[]>(
		this->terrain_data.terrain_id_count
	);
	this->terrain_data.terrain_id_blendmode_map = std::make_unique<int[]>(
		this->terrain_data.terrain_id_count
	);
	this->terrain_data.influences_buf           = std::make_unique<struct influence[]>(
		this->terrain_data.terrain_id_count
	);


	log::log(MSG(dbg) << "Terrain prefs: " <<
		"tiletypes=" << terrain_data.terrain_id_count << ", "
		"blendmodes=" << terrain_data.blendmode_count);

	// create tile textures (snow, ice, grass, whatever)
	for (size_t terrain_id = 0;
	     terrain_id < terrain_data.terrain_id_count;
	     terrain_id++) {

		auto line = &terrain_meta[terrain_id];

		// TODO: terrain double-define check?
		terrain_data.terrain_id_priority_map[terrain_id]  = line->blend_priority;
		terrain_data.terrain_id_blendmode_map[terrain_id] = line->blend_mode;

		// TODO: remove hardcoding and rely on nyan data
		auto terraintex_filename = util::sformat("converted/terrain/%d.slp.png",
		                                         line->slp_id);

		auto new_texture = this->assetmanager->get_texture(terraintex_filename, true);

		terrain_data.textures[terrain_id] = new_texture;
	}

	// create blending masks (see doc/media/blendomatic)
	for (size_t i = 0; i < terrain_data.blendmode_count; i++) {
		auto line = &blending_meta[i];

		// TODO: remove hardcodingn and use nyan data
		std::string mask_filename = util::sformat("converted/blendomatic/mode%02d.png",
		                                          line->blend_mode);
		terrain_data.blending_masks[i] = this->assetmanager->get_texture(mask_filename);
	}
}


void GameSpec::create_abilities(const gamedata::empiresdat &gamedata) {
	// use game data unit commands
	int headers =  gamedata.unit_headers.data.size();
	int total = 0;

	// it seems the index of the header indicates the unit
	for (int i = 0; i < headers; ++i) {

		// init unit command vector
		std::vector<const gamedata::unit_command *> list;

		// add each element
		auto &head = gamedata.unit_headers.data[i];
		for (auto &cmd : head.unit_commands.data) {
			total++;

			// commands either have a class id or a unit id
			// log::dbg("unit command %d %d -> class %d, unit %d, resource %d", i, cmd.id, cmd.class_id, cmd.unit_id, cmd.resource_in);
			list.push_back(&cmd);
		}

		// insert to command map
		this->commands[i] = list;
	}
}


void Sound::play() const {
	if (this->sound_items.size() <= 0) {
		return;
	}

	int rand = rng::random_range(0, this->sound_items.size());
	int sndid = this->sound_items.at(rand);

	try {
		// TODO: buhuuuu gnargghh this has to be moved to the asset loading subsystem hnnnng
		audio::AudioManager &am = this->game_spec->get_asset_manager()->get_engine()->get_audio_manager();

		if (not am.is_available()) {
			return;
		}

		audio::Sound sound = am.get_sound(audio::category_t::GAME, sndid);
		sound.play();
	}
	catch (audio::Error &e) {
		log::log(MSG(warn) << "cannot play: " << e);
	}
}

GameSpecHandle::GameSpecHandle(qtsdl::GuiItemLink *gui_link)
	:
	active{},
	asset_manager{},
	gui_signals{std::make_shared<GameSpecSignals>()},
	gui_link{gui_link} {
}

void GameSpecHandle::set_active(bool active) {
	this->active = active;

	this->start_loading_if_needed();
}

void GameSpecHandle::set_asset_manager(AssetManager *asset_manager) {
	if (this->asset_manager != asset_manager) {
		this->asset_manager = asset_manager;

		this->start_loading_if_needed();
	}
}

bool GameSpecHandle::is_ready() const {
	return this->spec && this->spec->load_complete();
}

void GameSpecHandle::invalidate() {
	this->spec = nullptr;

	if (this->asset_manager)
		this->asset_manager->check_updates();

	this->start_loading_if_needed();
}

void GameSpecHandle::announce_spec() {
	if (this->spec && this->spec->load_complete())
		emit this->gui_signals->game_spec_loaded(this->spec);
}

std::shared_ptr<GameSpec> GameSpecHandle::get_spec() {
	return this->spec;
}

void GameSpecHandle::start_loading_if_needed() {
	if (this->active && this->asset_manager && !this->spec) {

		// create the game specification
		this->spec = std::make_shared<GameSpec>(this->asset_manager);

		// the load the data
		this->start_load_job();
	}
}

void GameSpecHandle::start_load_job() {
	// store the shared pointers in another sharedptr
	// so we can pass them to the other thread
	auto spec_and_job = std::make_tuple(this->spec, this->gui_signals, job::Job<bool>{});
	auto spec_and_job_ptr = std::make_shared<decltype(spec_and_job)>(spec_and_job);

	// lambda to be executed to actually load the data files.
	auto perform_load = [spec_and_job_ptr] {
		return std::get<std::shared_ptr<GameSpec>>(*spec_and_job_ptr)->initialize();
	};

	auto load_finished = [gui_signals_ptr = this->gui_signals.get()] (job::result_function_t<bool> result) {
		bool load_ok;
		try {
			load_ok = result();
		}
		catch (Error &) {
			// TODO: display that error in the ui.
			throw;
		}
		catch (std::exception &) {
			// TODO: same here.
			throw Error{ERR << "gamespec loading failed!"};
		}

		if (load_ok) {
			// send the signal that the load job was finished
			emit gui_signals_ptr->load_job_finished();
		}
	};

	job::JobManager *job_mgr = this->asset_manager->get_engine()->get_job_manager();

	std::get<job::Job<bool>>(*spec_and_job_ptr) = job_mgr->enqueue<bool>(
		perform_load, load_finished
	);
}

} // openage
