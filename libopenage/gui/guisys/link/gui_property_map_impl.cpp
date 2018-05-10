// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_property_map_impl.h"

#include <QDynamicPropertyChangeEvent>
#include <QVariant>

#include "qtsdl_checked_static_cast.h"

namespace qtsdl {

GuiPropertyMapImpl::GuiPropertyMapImpl()
	:
	QObject{} {
}

GuiPropertyMapImpl::~GuiPropertyMapImpl() {
}

bool GuiPropertyMapImpl::event(QEvent *e) {
	if (e->type() == QEvent::DynamicPropertyChange) {
		auto property_name = checked_static_cast<QDynamicPropertyChangeEvent*>(e)->propertyName();
		emit this->property_changed(property_name, this->property(property_name));
		return true;
	}

	return this->QObject::event(e);
}

} // namespace qtsdl
