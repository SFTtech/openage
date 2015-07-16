// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "game_spec.h"
#include "generator.h"

namespace openage {


Generator::Generator(Engine *engine)
	:
	OptionNode{"Generator"},
	assetmanager{engine->get_data_dir()},
	spec{std::make_shared<GameSpec>(&this->assetmanager)} {

	// node settings
	this->set_parent(engine);
	this->add_int("generation_seed", &generation_seed);

	// some default players
	for (int i = 0; i < 8; ++i) {
		this->player_names.push_back("player" + std::to_string(i + 1));
	}

	options::OptionAction start_action("start", [this]() {
		this->create();
	});
	this->add_action(start_action);
}


std::shared_ptr<GameSpec> Generator::get_spec() const {
	return this->spec;
}

void Generator::create() {
	Engine &e = Engine::get();
	e.start_game(*this);
}


void lol() {
	// TODO: move these somewhere else
	//this->assetmanager.check_updates();
}

} // namespace openage