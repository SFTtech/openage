// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "game_saver.h"

#include <QtQml>

#include "../gamestate/game_save.h"
#include "../gamestate/game_main.h"
#include "../gamestate/generator.h"

#include "game_main_link.h"
#include "generator_link.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<GameSaver>("yay.sfttech.openage", 1, 0, "GameSaver");
}

GameSaver::GameSaver(QObject *parent)
	:
	QObject{parent},
	game{},
	generator_parameters{} {
	Q_UNUSED(registration);
}

GameSaver::~GameSaver() {
}

QString GameSaver::get_error_string() const {
	return this->error_string;
}

void GameSaver::activate() {
	static auto f = [] (GameMainHandle *game, Generator *generator, std::shared_ptr<GameSaverSignals> callback) {
		emit callback->error_message([&] {
			if (!game->is_game_running()) {
				return "no open game to save";
			} else {
				auto filename = generator->getv<std::string>("load_filename");
				gameio::save(game->get_game(), filename);
				return "";
			}
		}());
	};

	if (this->game && this->generator_parameters) {
		std::shared_ptr<GameSaverSignals> callback = std::make_shared<GameSaverSignals>();
		QObject::connect(callback.get(), &GameSaverSignals::error_message, this, &GameSaver::on_processed);

		this->game->i(f, this->generator_parameters, callback);
	} else {
		this->on_processed([this] {
			if (!this->game)
				return "provide 'game' before saving";
			if (!this->generator_parameters)
				return "provide 'generatorParameters' before saving";
			else
				ENSURE(false, "unhandled case for refusal to create a game");

			return "unknown error";
		}());
	}
}

void GameSaver::clearErrors() {
	this->error_string.clear();
	emit this->error_string_changed();
}

void GameSaver::on_processed(const QString &error_string) {
	this->error_string = error_string;
	emit this->error_string_changed();
}

}}  // namespace openage::gui
