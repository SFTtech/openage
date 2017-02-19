// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui_cvar_list_model.h"

#include <algorithm>

namespace openage {
namespace gui {

GuiCVarListModel::GuiCVarListModel(QObject *parent)
	:
	QAbstractListModel{parent} {

	QObject::connect(this, &GuiCVarListModel::property_changed, this, &GuiCVarListModel::on_property_changed);
}

GuiCVarListModel::~GuiCVarListModel() {
}

void GuiCVarListModel::set_cvar_manager(cvar::CVarManager *cvar_manager) {
	if (this->cvar_callback.manager() != cvar_manager) {
		this->beginResetModel();

		if (cvar_manager) {
			this->cvar_callback = cvar::CVarChangedCallback{*cvar_manager, [this](const std::string &name) { emit this->property_changed(name, QPrivateSignal{}); }};
			this->names = cvar_manager->get_names();
		} else {
			this->cvar_callback = cvar::CVarChangedCallback{};
		}

		this->endResetModel();
	}
}

void GuiCVarListModel::on_property_changed(const std::string &name) {
	auto foundIt = std::find(std::begin(this->names), std::end(this->names), name);

	if (foundIt != std::end(this->names)) {
		auto i = this->index(std::distance(std::begin(this->names), foundIt));
		emit this->dataChanged(i, i, {Qt::EditRole});
	} else {
		this->beginInsertRows(QModelIndex(), this->names.size(), this->names.size());
		this->names.emplace(std::end(this->names), name);
		this->endInsertRows();
	}
}

int GuiCVarListModel::rowCount(const QModelIndex&) const {
	return this->cvar_callback.manager() ? this->names.size() : 0;
}

Qt::ItemFlags GuiCVarListModel::flags(const QModelIndex &index) const {
	return Qt::ItemIsEditable | this->QAbstractListModel::flags(index);
}

QVariant GuiCVarListModel::data(const QModelIndex &index, int role) const {
	if (cvar::CVarManager *manager = this->cvar_callback.manager()) {
		switch (role) {
		case Qt::DisplayRole:
			return QByteArray::fromStdString(this->names[index.row()]);
			break;

		case Qt::EditRole: {
			return QString::fromStdString(manager->get(this->names[index.row()]));
			break;
		}

		default:
			break;
		}
	}

	return QVariant{};
}

bool GuiCVarListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (cvar::CVarManager *manager = this->cvar_callback.manager()) {
		if (role == Qt::EditRole) {
			manager->set(this->names[index.row()], value.toString().toStdString());
			return true;
		}
	}

	return false;
}

}} // namespace openage::gui
