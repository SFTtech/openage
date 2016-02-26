// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "game_creator.h"

#include <QtQml>

#include "../gamestate/game_main.h"
#include "../gamestate/game_spec.h"
#include "../gamestate/generator.h"

#include "game_main_link.h"
#include "game_spec_link.h"
#include "generator_link.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<GameCreator>("yay.sfttech.openage", 1, 0, "GameCreator");
}

GameCreator::GameCreator(QObject *parent)
	:
	QObject{parent},
	game{},
	game_spec{},
	generator_parameters{} {
	Q_UNUSED(registration);
}

GameCreator::~GameCreator() {
}

QString GameCreator::get_error_string() const {
	return this->error_string;
}

void GameCreator::activate() {
	static auto f = [] (GameMainHandle *game, GameSpecHandle *game_spec, Generator *generator, std::shared_ptr<GameCreatorSignals> callback) {
		emit callback->error_message([&] {
			if (game->is_game_running()) {
				return "close existing game before loading";
			} else if (!game_spec->is_ready()) {
				return "game data has not finished loading";
			} else if (auto game_main = generator->create(game_spec->get_spec())) {
				game->set_game(std::move(game_main));
				return "";
			} else {
				return "unknown error";
			}
		}());
	};

	if (this->game && this->game_spec && this->generator_parameters) {
		std::shared_ptr<GameCreatorSignals> callback = std::make_shared<GameCreatorSignals>();
		QObject::connect(callback.get(), &GameCreatorSignals::error_message, this, &GameCreator::on_processed);

		this->game->i(f, this->game_spec, this->generator_parameters, callback);
	} else {
		this->on_processed([this] {
			if (!this->game)
				return "provide 'game' before loading";
			if (!this->game_spec)
				return "provide 'gameSpec' before loading";
			if (!this->generator_parameters)
				return "provide 'generatorParameters' before loading";
			else
				ENSURE(false, "unhandled case for refusal to create a game");

			return "unknown error";
		}());
	}
}

void GameCreator::clearErrors() {
	this->error_string.clear();
	emit this->error_string_changed();
}

void GameCreator::on_processed(const QString &error_string) {
	this->error_string = error_string;
	emit this->error_string_changed();
}

}}  // namespace openage::gui
