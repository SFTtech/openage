// Copyright 2016 the openage authors. See copying.md for legal info.

#include "actions_list_model.h"

#include <QtQml>

#include "game_control_link.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<ActionsListModel>("yay.sfttech.openage", 1, 0, "ActionsListModel");
}

ActionsListModel::ActionsListModel(QObject *parent)
	:
	QAbstractListModel{parent},
	action_mode{} {
	Q_UNUSED(registration);
}

ActionsListModel::~ActionsListModel() {
}

ActionButtonsType ActionsListModel::get_active_buttons() const {
	return this->active_buttons;
}

void ActionsListModel::set_active_buttons(const ActionButtonsType &active_buttons) {
	if (this->active_buttons == active_buttons) {
		return;
	}
	this->active_buttons = active_buttons;

	switch (active_buttons) {
	case ActionButtonsType::MilitaryUnits:
		this->clear_buttons();
		this->beginInsertRows(QModelIndex(), 0, 9);
		this->add_button(6,  -1, static_cast<int>(GroupIDs::NoGroup),     "SET_ABILITY_PATROL");
		this->add_button(7,  -1, static_cast<int>(GroupIDs::NoGroup),     "SET_ABILITY_GUARD");
		this->add_button(8,  -1, static_cast<int>(GroupIDs::NoGroup),     "SET_ABILITY_FOLLOW");
		this->add_button(59, -1, static_cast<int>(GroupIDs::NoGroup),     "KILL_UNIT");
		this->add_button(2,  -1, static_cast<int>(GroupIDs::NoGroup),     "SET_ABILITY_GARRISON");

		this->add_button(9,  53, static_cast<int>(GroupIDs::StanceGroup), "AGGRESSIVE_STANCE");
		this->add_button(10, 52, static_cast<int>(GroupIDs::StanceGroup), "DEFENSIVE_STANCE");
		this->add_button(11, 51, static_cast<int>(GroupIDs::StanceGroup), "HOLD_STANCE");
		this->add_button(50, 54, static_cast<int>(GroupIDs::StanceGroup), "PASSIVE_STANCE");
		this->add_button(3,  -1, static_cast<int>(GroupIDs::NoGroup),     "STOP");
		this->endInsertRows();
		this->emit_actions_ready();
		break;

	case ActionButtonsType::CivilianUnits:
		this->clear_buttons();
		this->beginInsertRows(QModelIndex(), 0, 9);
		this->add_button(30, -1, static_cast<int>(GroupIDs::NoGroup), "BUILD_MENU");
		this->add_button(31, -1, static_cast<int>(GroupIDs::NoGroup), "BUILD_MENU_MIL");
		this->add_button(28, -1, static_cast<int>(GroupIDs::NoGroup), "REPAIR");
		this->add_button(59, -1, static_cast<int>(GroupIDs::NoGroup), "KILL_UNIT");
		this->add_button(2,  -1, static_cast<int>(GroupIDs::NoGroup), "SET_ABILITY_GARRISON");

		this->add_button(-1, -1, static_cast<int>(GroupIDs::NoGroup), "");
		this->add_button(-1, -1, static_cast<int>(GroupIDs::NoGroup), "");
		this->add_button(-1, -1, static_cast<int>(GroupIDs::NoGroup), "");
		this->add_button(-1, -1, static_cast<int>(GroupIDs::NoGroup), "");
		this->add_button(3,  -1, static_cast<int>(GroupIDs::NoGroup), "STOP");
		this->endInsertRows();
		this->emit_actions_ready();
		break;

	case ActionButtonsType::None:
		this->clear_buttons();
		this->emit_actions_ready();
		break;

	default:
		log::log(MSG(warn) << "Unknown action mode selection");
	}
}

ActionModeLink* ActionsListModel::get_action_mode() const {
	return this->action_mode;
}

void ActionsListModel::set_action_mode(ActionModeLink *action_mode) {
	if (this->action_mode != action_mode) {
		if (this->action_mode != nullptr) {
			QObject::disconnect(this->action_mode,
			                    &ActionModeLink::buttons_type_changed,
			                    this,
			                    &ActionsListModel::on_buttons_type_changed);
		}

		this->action_mode = action_mode;

		QObject::connect(this->action_mode,
		                 &ActionModeLink::buttons_type_changed,
		                 this,
		                 &ActionsListModel::on_buttons_type_changed);
	}
}

Q_INVOKABLE void ActionsListModel::set_initial_buttons() {
	this->set_active_buttons(ActionButtonsType::None);
}

void ActionsListModel::on_buttons_type_changed(const ActionButtonsType buttons_type) {
	this->set_active_buttons(buttons_type);
}

QHash<int, QByteArray> ActionsListModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[static_cast<int>(ActionsRoles::IconRole)] = "ico";
	roles[static_cast<int>(ActionsRoles::IconCheckedRole)] = "icoChk";
	roles[static_cast<int>(ActionsRoles::GroupIDRole)] = "grpID";
	roles[static_cast<int>(ActionsRoles::NameRole)] = "name";
	return roles;
}

int ActionsListModel::rowCount(const QModelIndex&) const {
	return this->buttons.size();
}

QVariant ActionsListModel::data(const QModelIndex &index, int role) const {
	return this->buttons.at(index.row()).value(role);
}

QMap<int, QVariant> ActionsListModel::itemData(const QModelIndex &index) const {
	return this->buttons.at(index.row());
}

void ActionsListModel::clear_buttons() {
	this->beginRemoveRows(QModelIndex(), 0, this->buttons.size() - 1);
	this->buttons.clear();
	this->endRemoveRows();
}

void ActionsListModel::add_button(int ico, int ico_chk, int grp_id, const char* name) {
	QMap<int, QVariant> map;
	map[static_cast<int>(ActionsRoles::IconRole)] = QVariant(ico);
	map[static_cast<int>(ActionsRoles::IconCheckedRole)] = QVariant(ico_chk);
	map[static_cast<int>(ActionsRoles::GroupIDRole)] = QVariant(grp_id);
	map[static_cast<int>(ActionsRoles::NameRole)] = QVariant(name);
	this->buttons.push_back(map);
}

void ActionsListModel::emit_actions_ready() {
	QVariant returned_value;
	QVariant parent_id = QQmlProperty::read(this->parent(), "id");
	QMetaObject::invokeMethod(this, "readyRoot", Q_RETURN_ARG(QVariant, returned_value));
}

}} // namespace openage::gui
