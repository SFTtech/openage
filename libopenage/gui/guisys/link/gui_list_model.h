// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <tuple>

#include <QAbstractListModel>

#include "gui_item.h"

namespace qtsdl {

/**
 * Adapts core that uses a property map to QAbstractListModel interface.
 */
class GuiListModel : public QAbstractListModel, public GuiItemBase, public GuiItemLink {
	Q_OBJECT

public:
	GuiListModel(QObject *parent=nullptr);
	virtual ~GuiListModel();

	void set(const std::vector<std::tuple<QByteArray, QVariant>> &values);

public slots:
	void on_property_changed(const QByteArray &name, const QVariant &value);

signals:
	void changed_from_gui(const char *name, const QVariant &value);

private:
	virtual int rowCount(const QModelIndex&) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

	std::vector<std::tuple<QByteArray, QVariant>> values;
};

} // namespace qtsdl
