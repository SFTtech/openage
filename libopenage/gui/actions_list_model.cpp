// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#include "actions_list_model.h"

#include "../log/log.h"

#include <QtQml>
#include <utility>

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<ActionsListModel>("yay.sfttech.openage", 1, 0, "ActionsListModel");
}

ActionsListModel::ActionsListModel(QObject *parent) :
	QAbstractListModel{parent} {
	Q_UNUSED(registration);
}

ActionsListModel::~ActionsListModel() = default;

QUrl ActionsListModel::get_icons_source() const {
	return QUrl(this->icons_source);
}

void ActionsListModel::set_icons_source(QUrl icons_source) {
	this->icons_source = std::move(icons_source);
}

void ActionsListModel::set_icons_source(const std::string &icons_source) {
	this->icons_source = QUrl(icons_source.c_str());
	emit this->icons_source_changed(this->icons_source);
}

QHash<int, QByteArray> ActionsListModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[static_cast<int>(ActionsRoles::IconRole)] = "ico";
	roles[static_cast<int>(ActionsRoles::IconCheckedRole)] = "icoChk";
	roles[static_cast<int>(ActionsRoles::GroupIDRole)] = "grpID";
	roles[static_cast<int>(ActionsRoles::NameRole)] = "name";
	return roles;
}

int ActionsListModel::rowCount(const QModelIndex &) const {
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

} // namespace gui
} // namespace openage
