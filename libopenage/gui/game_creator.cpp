// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "game_creator.h"

#include <QtQml>

#include "../gamestate/old/game_main.h"
#include "../gamestate/old/game_spec.h"
#include "../gamestate/old/generator.h"

#include "game_spec_link.h"
#include "generator_link.h"

namespace openage::gui {

namespace {
const int registration = qmlRegisterType<GameCreator>("yay.sfttech.openage", 1, 0, "GameCreator");
}

GameCreator::GameCreator(QObject *parent) :
	QObject{parent},
	generator_parameters{} {
	Q_UNUSED(registration);
}

GameCreator::~GameCreator() = default;

QString GameCreator::get_error_string() const {
	return this->error_string;
}

void GameCreator::activate() {
	static auto f = [](GameMainHandle *game,
	                   GameSpecHandle *game_spec,
	                   Generator *generator,
	                   std::shared_ptr<GameCreatorSignals> callback) {
		QString error_msg;

		if (game->is_game_running()) {
			error_msg = "close existing game before loading";
		}
		else if (not game_spec->is_ready()) {
			error_msg = "game data has not finished loading";
		}
		else {
			auto game_main = generator->create(game_spec->get_spec());

			if (game_main) {
				game->set_game(std::move(game_main));
			}
			else {
				error_msg = "unknown error";
			}
		}

		emit callback->error_message(error_msg);
	};
}

void GameCreator::clearErrors() {
	this->error_string.clear();
	emit this->error_string_changed();
}

void GameCreator::on_processed(const QString &error_string) {
	this->error_string = error_string;
	emit this->error_string_changed();
}

} // namespace openage::gui
