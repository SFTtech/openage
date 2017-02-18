// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "game_control_link.h"

#include <algorithm>

#include <QtQml>

#include "../engine.h"
#include "engine_link.h"
#include "game_main_link.h"

namespace openage {
namespace gui {

namespace {
const int registration_mode = qmlRegisterUncreatableType<OutputModeLink>("yay.sfttech.openage", 1, 0, "OutputMode", "OutputMode is an abstract interface for the concrete modes like EditorMode or ActionMode.");
const int registration_create = qmlRegisterType<CreateModeLink>("yay.sfttech.openage", 1, 0, "CreateMode");
const int registration_action = qmlRegisterType<ActionModeLink>("yay.sfttech.openage", 1, 0, "ActionMode");
const int registration_editor = qmlRegisterType<EditorModeLink>("yay.sfttech.openage", 1, 0, "EditorMode");
const int registration = qmlRegisterType<GameControlLink>("yay.sfttech.openage", 1, 0, "GameControl");
}

OutputModeLink::OutputModeLink(QObject *parent)
	:
	GuiItemQObject{parent},
	QQmlParserStatus{},
	GuiItemInterface<OutputModeLink>{} {
}

OutputModeLink::~OutputModeLink() {
}

QString OutputModeLink::get_name() const {
	return this->name;
}

QStringList OutputModeLink::get_binds() const {
	return this->binds;
}

void OutputModeLink::on_announced(const std::string &name) {
	auto new_name = QString::fromStdString(name);

	if (this->name != new_name) {
		this->name = new_name;
		emit this->name_changed();
	}
}

void OutputModeLink::on_binds_changed(
	const std::vector<std::string>& binds) {

	QStringList new_binds;
	std::transform(
		std::begin(binds),
		std::end(binds),
		std::back_inserter(new_binds),
		[] (const std::string &s) {
			return QString::fromStdString(s);
		}
	);

	if (this->binds != new_binds) {
		this->binds = new_binds;
		emit this->binds_changed();
	}
}

void OutputModeLink::classBegin() {
}

void OutputModeLink::on_core_adopted() {
	QObject::connect(&unwrap(this)->gui_signals,
	                 &OutputModeSignals::announced,
	                 this,
	                 &OutputModeLink::on_announced);

	QObject::connect(&unwrap(this)->gui_signals,
	                 &OutputModeSignals::binds_changed,
	                 this,
	                 &OutputModeLink::on_binds_changed);
}

void OutputModeLink::componentComplete() {
	static auto f = [] (OutputMode *_this) {
		_this->announce();
	};
	this->i(f);
}

CreateModeLink::CreateModeLink(QObject *parent)
	:
	Inherits{parent} {
	Q_UNUSED(registration_create);
}

CreateModeLink::~CreateModeLink() {
}

ActionModeLink::ActionModeLink(QObject *parent)
	:
	Inherits{parent} {
	Q_UNUSED(registration_action);
}

ActionModeLink::~ActionModeLink() {
}

QString ActionModeLink::get_ability() const {
	return this->ability;
}

QString ActionModeLink::get_population() const {
	return this->population;
}

void ActionModeLink::act(const QString &action) {
	emit this->action_triggered(action.toStdString());
}

void ActionModeLink::on_ability_changed(const std::string &ability) {
	this->ability = QString::fromStdString(ability);
	emit this->ability_changed();
}

void ActionModeLink::on_buttons_type_changed(const ActionButtonsType buttons_type) {
	emit this->buttons_type_changed(buttons_type);
}

void ActionModeLink::on_population_changed(int population, int population_cap) {
	this->population = QString::number(population) + "/" + QString::number(population_cap);
	emit this->population_changed();
}

void ActionModeLink::on_core_adopted() {
	this->Inherits::on_core_adopted();
	QObject::connect(&unwrap(this)->gui_signals,
	                 &ActionModeSignals::ability_changed,
	                 this,
	                 &ActionModeLink::on_ability_changed);
	QObject::connect(&unwrap(this)->gui_signals,
	                 &ActionModeSignals::population_changed,
	                 this,
	                 &ActionModeLink::on_population_changed);
	QObject::connect(&unwrap(this)->gui_signals,
	                 &ActionModeSignals::buttons_type_changed,
	                 this,
	                 &ActionModeLink::on_buttons_type_changed);
	QObject::connect(this,
	                 &ActionModeLink::action_triggered,
	                 &unwrap(this)->gui_signals,
	                 &ActionModeSignals::on_action);
}

EditorModeLink::EditorModeLink(QObject *parent)
	:
	Inherits{parent},
	current_type_id{-1},
	current_terrain_id{-1},
	paint_terrain{} {
	Q_UNUSED(registration_editor);
}

EditorModeLink::~EditorModeLink() {
}

int EditorModeLink::get_current_type_id() const {
	return this->current_type_id;
}

void EditorModeLink::set_current_type_id(int current_type_id) {
	static auto f = [](EditorMode* _this, int current_type_id) {
		_this->set_current_type_id(current_type_id);
	};
	this->s(f, this->current_type_id, current_type_id);
}

int EditorModeLink::get_current_terrain_id() const {
	return this->current_terrain_id;
}

void EditorModeLink::set_current_terrain_id(int current_terrain_id) {
	static auto f = [](EditorMode* _this, int current_terrain_id) {
		_this->set_current_terrain_id(current_terrain_id);
	};
	this->s(f, this->current_terrain_id, current_terrain_id);
}

bool EditorModeLink::get_paint_terrain() const {
	return this->paint_terrain;
}

void EditorModeLink::set_paint_terrain(bool paint_terrain) {
	static auto f = [](EditorMode* _this, int paint_terrain) {
		_this->set_paint_terrain(paint_terrain);
	};
	this->s(f, this->paint_terrain, paint_terrain);
}

QStringList EditorModeLink::get_categories() const {
	return this->categories;
}

void EditorModeLink::announce_category_content(const std::string &category_name) {
	static auto f = [] (EditorMode *_this, const std::string &category_name) {
		_this->announce_category_content(category_name);
	};
	this->i(f, category_name);
}

void EditorModeLink::on_categories_changed(const std::vector<std::string> &categories) {
	this->categories.clear();
	std::transform(std::begin(categories), std::end(categories), std::back_inserter(this->categories), [] (const std::string &s) {return QString::fromStdString(s);});
	emit this->categories_changed();
}

void EditorModeLink::on_core_adopted() {
	this->Inherits::on_core_adopted();
	QObject::connect(&unwrap(this)->gui_signals, &EditorModeSignals::toggle, this, &EditorModeLink::toggle);
	QObject::connect(&unwrap(this)->gui_signals, &EditorModeSignals::categories_changed, this, &EditorModeLink::on_categories_changed);
	QObject::connect(&unwrap(this)->gui_signals, &EditorModeSignals::categories_content_changed, this, &EditorModeLink::categories_content_changed);
	QObject::connect(&unwrap(this)->gui_signals, &EditorModeSignals::category_content_changed, this, &EditorModeLink::category_content_changed);
}

GameControlLink::GameControlLink(QObject *parent)
	:
	GuiItemQObject{parent},
	QQmlParserStatus{},
	GuiItem{this},
	mode{},
	effective_mode_index{-1},
	mode_index{-1},
	engine{},
	game{},
	current_civ_index{} {
	Q_UNUSED(registration_mode);
	Q_UNUSED(registration);
}

GameControlLink::~GameControlLink() {
}

void GameControlLink::classBegin() {
}

void GameControlLink::on_core_adopted() {
	QObject::connect(&unwrap(this)->gui_signals, &GameControlSignals::mode_changed, this, &GameControlLink::on_mode_changed);
	QObject::connect(&unwrap(this)->gui_signals, &GameControlSignals::modes_changed, this, &GameControlLink::on_modes_changed);
	QObject::connect(&unwrap(this)->gui_signals, &GameControlSignals::current_player_name_changed, this, &GameControlLink::on_current_player_name_changed);
	QObject::connect(&unwrap(this)->gui_signals, &GameControlSignals::current_civ_index_changed, this, &GameControlLink::on_current_civ_index_changed);
}

void GameControlLink::componentComplete() {
	static auto f = [] (GameControl *_this) {
		_this->announce_mode();
		_this->announce_current_player_name();
	};
	this->i(f);
}

OutputModeLink* GameControlLink::get_mode() const {
	return this->mode;
}

int GameControlLink::get_effective_mode_index() const {
	return this->effective_mode_index;
}

int GameControlLink::get_mode_index() const {
	return this->mode_index;
}

void GameControlLink::set_mode_index(int mode) {
	static auto f = [] (GameControl *_this, int mode) {
		_this->set_mode(mode, true);
	};

	this->sf(f, this->mode_index, mode);
}

QVariantList GameControlLink::get_modes() const {
	return this->modes;
}

void GameControlLink::set_modes(const QVariantList &modes) {
	static auto f = [] (GameControl *_this, const QVariantList &modes) {
		std::vector<OutputMode*> new_modes;

		for (auto m : modes)
			if (m.canConvert<OutputModeLink*>())
				new_modes.push_back(unwrap(m.value<OutputModeLink*>()));

		_this->set_modes(new_modes);
	};

	this->s(f, this->modes, modes);
}

EngineLink* GameControlLink::get_engine() const {
	return this->engine;
}

void GameControlLink::set_engine(EngineLink *engine) {
	static auto f = [] (GameControl *_this, Engine *engine) {
		_this->set_engine(engine);
	};
	this->s(f, this->engine, engine);
}

GameMainLink* GameControlLink::get_game() const {
	return this->game;
}

void GameControlLink::set_game(GameMainLink *game) {
	static auto f = [] (GameControl *_this, GameMainHandle *game) {
		_this->set_game(game);
	};
	this->s(f, this->game, game);
}

QString GameControlLink::get_current_player_name() const {
	return this->current_player_name;
}

int GameControlLink::get_current_civ_index() const {
	return this->current_civ_index;
}

void GameControlLink::on_mode_changed(OutputMode *mode, int mode_index) {
	auto new_mode = qtsdl::wrap(mode);

	if (this->mode != new_mode || this->effective_mode_index != mode_index) {
		this->effective_mode_index = mode_index;
		this->mode = new_mode;
		emit this->mode_changed();
	}
}

void GameControlLink::on_modes_changed(OutputMode *mode, int mode_index) {
	static auto f = [] (GameControl *_this, int mode) {
		_this->set_mode(mode);
	};
	this->i(f, this->mode_index);

	this->on_mode_changed(mode, mode_index);
	emit this->modes_changed();
}

void GameControlLink::on_current_player_name_changed(const std::string &current_player_name) {
	this->current_player_name = QString::fromStdString(current_player_name);
	emit this->current_player_name_changed();
}

void GameControlLink::on_current_civ_index_changed(int current_civ_index) {
	this->current_civ_index = current_civ_index;
	emit this->current_civ_index_changed();
}

}} // namespace openage::gui
