// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include <QAbstractListModel>
#include <QMap>
#include <QUrl>

namespace openage {
namespace gui {

/**
 * Model used for the Action Buttons to render (e.g. for civilian units,
 * military units, buildings etc.)
 */
class ActionsListModel : public QAbstractListModel {
	Q_OBJECT

	Q_PROPERTY(QUrl iconsSource READ get_icons_source WRITE set_icons_source NOTIFY icons_source_changed)

public:
	ActionsListModel(QObject *parent = nullptr);
	virtual ~ActionsListModel();

	QUrl get_icons_source() const;
	void set_icons_source(QUrl icons_source);

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

signals:
	void icons_source_changed(const QUrl icons_source);

private:
	virtual QHash<int, QByteArray> roleNames() const override;
	virtual int rowCount(const QModelIndex &) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QMap<int, QVariant> itemData(const QModelIndex &index) const override;

	/**
	 * Utility function to create a QUrl from a string and set it as iconsSource
	 */
	void set_icons_source(const std::string &icons_source);

	/**
	 * Clears all buttons
	 */
	void clear_buttons();

	/**
	 * Shortcut to creating a QMap for a button
	 */
	void add_button(int ico, int ico_chk, int grp_id, const char *name);

	QUrl icons_source;
	std::vector<QMap<int, QVariant>> buttons;
};

} // namespace gui
} // namespace openage
