// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "category_contents_list_model.h"

#include <QtQml>

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<CategoryContentsListModel>("yay.sfttech.openage", 1, 0, "Category");
}

CategoryContentsListModel::CategoryContentsListModel(QObject *parent) :
	QAbstractListModel{parent} {
	Q_UNUSED(registration);
}

CategoryContentsListModel::~CategoryContentsListModel() = default;

QString CategoryContentsListModel::get_name() const {
	return this->name;
}

void CategoryContentsListModel::set_name(const QString &name) {
	if (this->name != name) {
		this->name = name;

		this->on_categories_content_changed();
	}
}

void CategoryContentsListModel::on_category_content_changed(const std::string &category_name, std::vector<std::tuple<index_t, uint16_t>> type_and_texture) {
	if (this->name == QString::fromStdString(category_name)) {
		this->beginResetModel();
		this->type_and_texture = type_and_texture;
		this->endResetModel();
	}
}

void CategoryContentsListModel::on_categories_content_changed() {
}

QHash<int, QByteArray> CategoryContentsListModel::roleNames() const {
	auto names = this->QAbstractListModel::roleNames();
	names.insert(Qt::UserRole + 1, "typeId");
	return names;
}

int CategoryContentsListModel::rowCount(const QModelIndex &) const {
	return this->type_and_texture.size();
}

QVariant CategoryContentsListModel::data(const QModelIndex &index, int role) const {
	switch (role) {
	case Qt::DisplayRole:
		return std::get<1>(this->type_and_texture[index.row()]);

	case Qt::UserRole + 1:
		return std::get<0>(this->type_and_texture[index.row()]);

	default:
		break;
	}

	return QVariant{};
}

} // namespace gui
} // namespace openage
