// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "game_spec_link.h"

#include <QtQml>

namespace openage::gui {

namespace {
const int registration = qmlRegisterType<GameSpecLink>("yay.sfttech.openage", 1, 0, "GameSpec");
const int registration_of_ptr = qRegisterMetaType<std::shared_ptr<GameSpec>>("shared_ptr");
} // namespace

GameSpecLink::GameSpecLink(QObject *parent) :
	GuiItemQObject{parent},
	QQmlParserStatus{},
	GuiItem{this},
	state{},
	active{},
	terrain_id_count{} {
	Q_UNUSED(registration);
	Q_UNUSED(registration_of_ptr);
}

GameSpecLink::~GameSpecLink() = default;

void GameSpecLink::classBegin() {
}

void GameSpecLink::on_core_adopted() {
	auto core = unwrap(this);
	QObject::connect(core->gui_signals.get(), &GameSpecSignals::load_job_finished, this, &GameSpecLink::on_load_job_finished);
	QObject::connect(core->gui_signals.get(), &GameSpecSignals::game_spec_loaded, this, &GameSpecLink::on_game_spec_loaded);
}

void GameSpecLink::componentComplete() {
	this->on_load_job_finished();
}

void GameSpecLink::on_load_job_finished() {
	static auto f = [](GameSpecHandle *_this) {
		_this->announce_spec();
	};
	this->i(f);
}

void GameSpecLink::on_game_spec_loaded(std::shared_ptr<GameSpec> loaded_game_spec) {
	this->loaded_game_spec = loaded_game_spec;

	this->terrain_id_count = this->loaded_game_spec->get_terrain_meta()->terrain_id_count;

	this->state = State::Ready;

	emit this->state_changed();
	emit this->terrain_id_count_changed();
	emit this->game_spec_loaded(this, this->loaded_game_spec);
}

std::shared_ptr<GameSpec> GameSpecLink::get_loaded_spec() {
	return this->loaded_game_spec;
}

GameSpecLink::State GameSpecLink::get_state() const {
	return this->state;
}

void GameSpecLink::invalidate() {
	static auto f = [](GameSpecHandle *_this) {
		_this->invalidate();
	};
	this->i(f);

	this->set_state(this->active ? State::Loading : State::Null);
}

bool GameSpecLink::get_active() const {
	return this->active;
}

void GameSpecLink::set_active(bool active) {
	static auto f = [](GameSpecHandle *_this, bool active) {
		_this->set_active(active);
	};
	this->s(f, this->active, active);

	this->set_state(this->active && this->state == State::Null ? State::Loading : this->state);
}

int GameSpecLink::get_terrain_id_count() const {
	return this->terrain_id_count;
}

void GameSpecLink::set_state(GameSpecLink::State state) {
	if (state != this->state) {
		this->state = state;
		emit this->state_changed();
	}
}

} // namespace openage::gui
