// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GENERATOR_H_
#define OPENAGE_GENERATOR_H_

#include "assetmanager.h"
#include "options.h"

namespace openage {

class GameSpec;

/**
 * Manages creation and setup of new games
 *
 * required values used to construct a game
 * this includes game spec and players
 *
 * this will be identical for each networked
 * player in a game
 */
class Generator : public options::OptionNode {
public:
	Generator(Engine *engine);

	std::shared_ptr<GameSpec> get_spec() const;

	int generation_seed;

	// TODO use vector of players
	std::vector<std::string> player_names;

	void create();

private:

	// access to games asset files
	AssetManager assetmanager;

	// data version used to create a game
	std::shared_ptr<GameSpec> spec;

};

} // namespace openage

#endif
