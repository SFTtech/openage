// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <memory>

#include "../gamestate/types.h"

#include <QAbstractListModel>

namespace openage {
namespace gui {

class EditorModeLink;

/**
 * Adaptor for the contents of a category of the Civilisation.
 */
class CategoryContentsListModel : public QAbstractListModel {
	Q_OBJECT

	Q_PROPERTY(QString name READ get_name WRITE set_name)
	Q_PROPERTY(openage::gui::EditorModeLink* editorMode READ get_editor_mode WRITE set_editor_mode)

public:
	CategoryContentsListModel(QObject *parent=nullptr);
	virtual ~CategoryContentsListModel();

	QString get_name() const;
	void set_name(const QString &name);

	EditorModeLink* get_editor_mode() const;
	void set_editor_mode(EditorModeLink *editor_mode);

private slots:
	void on_category_content_changed(const std::string &category_name, std::vector<std::tuple<index_t, uint16_t>> type_and_texture);
	void on_categories_content_changed();

private:
	virtual QHash<int, QByteArray> roleNames() const override;
	virtual int rowCount(const QModelIndex&) const override;
	virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

	std::vector<std::tuple<index_t, uint16_t>> type_and_texture;

	QString name;
	EditorModeLink *editor_mode;
};

}} // namespace openage::gui
