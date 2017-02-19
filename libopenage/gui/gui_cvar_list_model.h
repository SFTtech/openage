// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <string>

#include <QAbstractListModel>

#include "../cvar/cvar.h"

#include "guisys/link/gui_item.h"

namespace openage {
namespace gui {

/**
 * Adapts CVarManager to QAbstractListModel interface.
 */
class GuiCVarListModel : public QAbstractListModel, public qtsdl::GuiItemBase, public qtsdl::GuiItemLink {
	Q_OBJECT

public:
	GuiCVarListModel(QObject *parent=nullptr);
	virtual ~GuiCVarListModel();

	/**
	 * cvar::CVarManager must be MT safe
	 */
	void set_cvar_manager(cvar::CVarManager *cvar_manager);

public slots:
	void on_property_changed(const std::string &name);

signals:
	void changed_from_gui(const std::string &name, const std::string &value);
	void property_changed(const std::string &name, QPrivateSignal);

private:
	virtual int rowCount(const QModelIndex&) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

	std::vector<std::string> names;
	cvar::CVarChangedCallback cvar_callback;
};

}} // namespace openage::gui
