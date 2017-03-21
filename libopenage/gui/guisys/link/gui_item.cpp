// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "gui_item.h"

namespace qtsdl {


QString name_tidier(const char *name) {
	QString cleaner_name = QString::fromLatin1(name);
	cleaner_name.remove(QRegularExpression("qtsdl|PersistentCoreHolder"));
	return cleaner_name;
}

GuiItemQObject::GuiItemQObject(QObject *parent)
	:
	QObject{parent},
	GuiItemBase{} {
}

} // namespace qtsdl
