// Copyright 2016-2019 the openage authors. See copying.md for legal info.

#include "game_saver.h"

#include <QtQml>

#include "../gamestate/old/game_save.h"
#include "../gamestate/old/game_main.h"
#include "../gamestate/old/generator.h"

#include "game_main_link.h"
#include "generator_link.h"

namespace openage::gui {

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

GameSaver::~GameSaver() = default;

QString GameSaver::get_error_string() const {
	return this->error_string;
}

// called when the save-game button is pressed:
void GameSaver::activate() {
	static auto f = [] (GameMainHandle *game,
	                    Generator *generator,
	                    std::shared_ptr<GameSaverSignals> callback) {

		QString error_msg;

		if (!game->is_game_running()) {
			error_msg = "no open game to save";
		} else {
			auto filename = generator->getv<std::string>("load_filename");
			gameio::save(game->get_game(), filename);
		}

		emit callback->error_message(error_msg);
	};

	if (this->game && this->generator_parameters) {
		std::shared_ptr<GameSaverSignals> callback = std::make_shared<GameSaverSignals>();
		QObject::connect(callback.get(),
		                 &GameSaverSignals::error_message,
		                 this,
		                 &GameSaver::on_processed);

		this->game->i(f, this->generator_parameters, callback);
	}
	else {
		QString error_msg = "unknown error";

		if (!this->game) {
			error_msg = "provide 'game' before saving";
		}

		if (!this->generator_parameters) {
			error_msg = "provide 'generatorParameters' before saving";
		}
		else {
			ENSURE(false, "unhandled case for refusal to create a game");
		}

		this->on_processed(error_msg);
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

}  // namespace openage::gui
