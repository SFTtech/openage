// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

#include "renderer/gui/guisys/link/gui_item_link.h"


namespace qtgui {

class GuiSingletonItem : public QObject
	, public GuiItemLink {
	Q_OBJECT

public:
	explicit GuiSingletonItem(QObject *parent = nullptr);
	virtual ~GuiSingletonItem();
};

} // namespace qtgui
