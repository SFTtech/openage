// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

#include "gui_item_link.h"

namespace qtsdl {

class GuiSingletonItem : public QObject, public GuiItemLink {
	Q_OBJECT

public:
	explicit GuiSingletonItem(QObject *parent=nullptr);
	virtual ~GuiSingletonItem();
};

} // namespace qtsdl
