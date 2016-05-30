// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_item.h"

namespace qtsdl {

GuiItemQObject::GuiItemQObject(QObject *parent)
	:
	QObject{parent},
	GuiItemBase{} {
}

} // namespace qtsdl
