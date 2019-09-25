// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_list_model.h"

#include <algorithm>

namespace qtsdl {

GuiListModel::GuiListModel(QObject *parent)
	:
	QAbstractListModel{parent} {
}

GuiListModel::~GuiListModel() {
}

void GuiListModel::set(const std::vector<std::tuple<QByteArray, QVariant>> &values) {
	this->beginResetModel();
	this->values = values;
	this->endResetModel();
}

void GuiListModel::on_property_changed(const QByteArray &name, const QVariant &value) {
	auto foundIt = std::find_if(std::begin(this->values), std::end(this->values), [&name] (std::tuple<QByteArray, QVariant>& p) {
		return std::get<QByteArray>(p) == name;
	});

	if (foundIt != std::end(this->values)) {
		auto &current_value = std::get<QVariant>(*foundIt);

		if (current_value != value) {
			current_value = value;

			auto i = this->index(std::distance(std::begin(this->values), foundIt));
			emit this->dataChanged(i, i, {Qt::EditRole});
		}
	} else {
		this->beginInsertRows(QModelIndex(), this->values.size(), this->values.size());
		this->values.emplace(std::end(this->values), name, value);
		this->endInsertRows();
	}
}

int GuiListModel::rowCount(const QModelIndex&) const {
	return values.size();
}

Qt::ItemFlags GuiListModel::flags(const QModelIndex &index) const {
	return Qt::ItemIsEditable | this->QAbstractListModel::flags(index);
}

QVariant GuiListModel::data(const QModelIndex &index, int role) const {
	switch (role) {
	case Qt::DisplayRole:
		return std::get<QByteArray>(this->values[index.row()]);

	case Qt::EditRole:
		return std::get<QVariant>(this->values[index.row()]);

	default:
		return QVariant{};
	}
}

bool GuiListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (role == Qt::EditRole) {
		auto &property = this->values[index.row()];
		emit this->changed_from_gui(std::get<QByteArray>(property), std::get<QVariant>(property) = value);
		return true;
	}

	return false;
}

} // namespace qtsdl
