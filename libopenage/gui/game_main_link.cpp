// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "game_main_link.h"

#include <QtQml>

#include "../engine.h"
#include "engine_link.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<GameMainLink>("yay.sfttech.openage", 1, 0, "GameMain");
}

GameMainLink::GameMainLink(QObject *parent)
	:
	GuiItemQObject{parent},
	QQmlParserStatus{},
	GuiItem{this},
	state{},
	active{},
	engine{} {
	Q_UNUSED(registration);
}

GameMainLink::~GameMainLink() {
}

void GameMainLink::classBegin() {
}

void GameMainLink::on_core_adopted() {
	QObject::connect(&unwrap(this)->gui_signals, &GameMainSignals::game_running, this, &GameMainLink::on_game_running);
}

void GameMainLink::componentComplete() {
	static auto f = [] (GameMainHandle *_this) {
		_this->announce_running();
	};
	this->i(f);
}

GameMainLink::State GameMainLink::get_state() const {
	return this->state;
}

EngineLink* GameMainLink::get_engine() const {
	return this->engine;
}

void GameMainLink::set_engine(EngineLink *engine) {
	static auto f = [] (GameMainHandle *_this, Engine *engine) {
		_this->set_engine(engine);
	};
	this->s(f, this->engine, engine);
}

void GameMainLink::clear() {
	static auto f = [] (GameMainHandle *_this) {
		_this->clear();
	};
	this->i(f);
}

void GameMainLink::on_game_running(bool running) {
	auto state = running ? State::Running : State::Null;

	if (this->state != state) {
		this->state = state;
		emit this->state_changed();
	}
}

}} // namespace openage::gui
