// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <tuple>

#include <QAbstractListModel>

#include "../gamestate/resource.h"

namespace openage {
namespace gui {

class ActionModeLink;

/**
 * Resource table for the gui.
 */
class ResourcesListModel : public QAbstractListModel {
	Q_OBJECT

	Q_PROPERTY(openage::gui::ActionModeLink* actionMode READ get_action_mode WRITE set_action_mode)

public:
	ResourcesListModel(QObject *parent=nullptr);
	virtual ~ResourcesListModel();

	ActionModeLink* get_action_mode() const;
	void set_action_mode(ActionModeLink *action_mode);

private slots:
	void on_resource_changed(game_resource resource, int amount);

private:
	virtual int rowCount(const QModelIndex&) const override;
	virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

	std::vector<int> amounts;

	ActionModeLink *action_mode;
};

}} // namespace openage::gui
