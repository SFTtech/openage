// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_singleton_item.h"


namespace qtgui {

GuiSingletonItem::GuiSingletonItem(QObject *parent) :
	QObject{parent},
	GuiItemLink{} {
}

GuiSingletonItem::~GuiSingletonItem() = default;

} // namespace qtgui
