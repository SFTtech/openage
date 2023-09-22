// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <tuple>
#include <vector>

#include <QAbstractListModel>

#include "../gamestate/old/resource.h"

namespace openage {
namespace gui {

/**
 * Resource table for the gui.
 */
class ResourcesListModel : public QAbstractListModel {
	Q_OBJECT

public:
	ResourcesListModel(QObject *parent = nullptr);
	virtual ~ResourcesListModel();

private slots:
	void on_resource_changed(game_resource resource, int amount);

private:
	virtual int rowCount(const QModelIndex &) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	std::vector<int> amounts;
};

} // namespace gui
} // namespace openage
