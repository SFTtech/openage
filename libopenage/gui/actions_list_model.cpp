// Copyright 2016-2016 the openage authors. See copying.md for legal info.

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
	case ActionButtonsType::None:
		this->clear_buttons();
		break;

	case ActionButtonsType::MilitaryUnits:
		this->clear_buttons();
		this->set_icons_source("image://by-filename/converted/interface/hudactions.slp.png");
		this->beginResetModel();
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
		this->endResetModel();
		break;

	case ActionButtonsType::CivilianUnits:
		this->clear_buttons();
		this->set_icons_source("image://by-filename/converted/interface/hudactions.slp.png");
		this->beginResetModel();
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
		this->endResetModel();
		break;

	case ActionButtonsType::BuildMenu:
		this->clear_buttons();
		this->set_icons_source("image://by-filename/converted/interface/50705.slp.png");
		this->beginResetModel();
		this->add_button(34, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_HOUS");
		this->add_button(20, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_MILL");
		this->add_button(39, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_MINE");
		this->add_button(40, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_SMIL");
		this->add_button(13, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_DOCK");
		this->add_button(35, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_FARM");
		this->add_button(4,  -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_BLAC");
		this->add_button(16, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_MRKT");
		this->add_button(10, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_CRCH");
		this->add_button(32, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_UNIV");
		this->add_button(28, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_RTWC");
		this->add_button(37, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_WNDR");
		this->endResetModel();
		break;

	case ActionButtonsType::MilBuildMenu:
		this->clear_buttons();
		this->set_icons_source("image://by-filename/converted/interface/50705.slp.png");
		this->beginResetModel();
		this->add_button(2,  -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_BRKS");
		this->add_button(0,  -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_ARRG");
		this->add_button(23, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_STBL");
		this->add_button(22, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_SIWS");
		this->add_button(38, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_WCTWX");
		this->add_button(30, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_WALL");
		this->add_button(29, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_WALL2");
		this->add_button(25, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_WCTW");
		this->add_button(42, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_WCTW4");
		this->add_button(36, -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_GTCA2");
		this->add_button(7,  -1, static_cast<int>(GroupIDs::NoGroup), "BUILDING_CSTL");
		this->endResetModel();
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
	this->beginResetModel();
	this->buttons.clear();
	this->endResetModel();
}

void ActionsListModel::add_button(int ico, int ico_chk, int grp_id, const char *name) {
	QMap<int, QVariant> map;
	map[static_cast<int>(ActionsRoles::IconRole)] = QVariant(ico);
	map[static_cast<int>(ActionsRoles::IconCheckedRole)] = QVariant(ico_chk);
	map[static_cast<int>(ActionsRoles::GroupIDRole)] = QVariant(grp_id);
	map[static_cast<int>(ActionsRoles::NameRole)] = QVariant(name);
	this->buttons.push_back(map);
}

void ActionsListModel::set_icons_source(const char *source) {
	this->parent()->parent()->setProperty("iconsSource", QVariant(source));
}

}} // namespace openage::gui
