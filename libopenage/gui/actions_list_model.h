// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "game_control_link.h"

#include <QAbstractListModel>
#include <QMap>

namespace openage {
namespace gui {

/**
 * Model used for the Action Buttons to render (e.g. for civilian units,
 * military units, buildings etc.)
 */
class ActionsListModel : public QAbstractListModel {
	Q_OBJECT

	Q_PROPERTY(ActionButtonsType active_buttons READ get_active_buttons WRITE set_active_buttons)
	Q_PROPERTY(ActionModeLink* action_mode READ get_action_mode WRITE set_action_mode)

public:
	ActionsListModel(QObject *parent=nullptr);
	virtual ~ActionsListModel();

	ActionButtonsType get_active_buttons() const;
	void set_active_buttons(const ActionButtonsType &active_buttons);

	ActionModeLink* get_action_mode() const;
	void set_action_mode(ActionModeLink* action_mode);

	Q_INVOKABLE void set_initial_buttons();

	enum class ActionsRoles {
		IconRole = Qt::UserRole + 1,
		IconCheckedRole,
		GroupIDRole,
		NameRole
	};

	enum class GroupIDs {
		NoGroup,
		StanceGroup
	};

private slots:
	void on_buttons_type_changed(const ActionButtonsType buttons_type);

private:
	virtual QHash<int, QByteArray> roleNames() const override;
	virtual int rowCount(const QModelIndex&) const override;
	virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
	virtual QMap<int, QVariant> itemData(const QModelIndex &index) const override;

	/**
	 * Clears all buttons
	 */
	void clear_buttons();

	/**
	 * Shortcut to creating a QMap for a button
	 */
	void add_button(int ico, int ico_chk, int grp_id, const char* name);

	/**
	 * Emit the ready signal of the parent's parent (the Actions QML object)
	 */
	void emit_actions_ready();

	ActionButtonsType active_buttons;
	ActionModeLink *action_mode;
	std::vector<QMap<int, QVariant>> buttons;
};

}} // namespace openage::gui
