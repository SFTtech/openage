// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#include "resources_list_model.h"

#include <iterator>
#include <type_traits>

#include <QtQml>

#include "../error/error.h"

#include "../game_control.h"
#include "game_control_link.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<ResourcesListModel>("yay.sfttech.openage", 1, 0, "Resources");

const auto resource_type_count = static_cast<std::underlying_type<game_resource>::type>(game_resource::RESOURCE_TYPE_COUNT);

}

ResourcesListModel::ResourcesListModel(QObject *parent)
	:
	QAbstractListModel(parent),
	amounts(resource_type_count),
	action_mode() {
	Q_UNUSED(registration);
}

ResourcesListModel::~ResourcesListModel() {
}

ActionModeLink* ResourcesListModel::get_action_mode() const {
	return this->action_mode;
}

void ResourcesListModel::set_action_mode(ActionModeLink *action_mode) {
	if (this->action_mode != action_mode) {
		if (this->action_mode) {
			QObject::disconnect(&unwrap(this->action_mode)->gui_signals, &ActionModeSignals::resource_changed, this, &ResourcesListModel::on_resource_changed);
		}

		this->action_mode = action_mode;

		if (this->action_mode) {
			QObject::connect(&unwrap(this->action_mode)->gui_signals, &ActionModeSignals::resource_changed, this, &ResourcesListModel::on_resource_changed);
		}
	}
}

void ResourcesListModel::on_resource_changed(game_resource resource, int amount) {
	int i = static_cast<int>(resource);
	ENSURE(i >= 0 && i < std::distance(std::begin(this->amounts), std::end(this->amounts)), "Res type index is out of range: '" << i << "'.");

	if (this->amounts[i] != amount) {
		auto model_index = this->index(i);

		this->amounts[i] = amount;
		emit this->dataChanged(model_index, model_index, QVector<int>{Qt::DisplayRole});
	}
}

int ResourcesListModel::rowCount(const QModelIndex&) const {
	ENSURE(resource_type_count == this->amounts.size(), "Res type count is compile-time const '" << resource_type_count << "', but got '" << this->amounts.size() << "'.");
	return this->amounts.size();
}

QVariant ResourcesListModel::data(const QModelIndex &index, int role) const {
	const int i = index.row();

	switch (role) {
	case Qt::DisplayRole:
		ENSURE(i >= 0 && i < std::distance(std::begin(this->amounts), std::end(this->amounts)), "Res type index is out of range: '" << i << "'.");
		return this->amounts[index.row()];
		break;

	default:
		break;
	}

	return QVariant{};
}

}} // namespace openage::gui
