// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "minimap_marker_type.h"

#include "../error/error.h"

namespace openage {
namespace gui {

namespace {
const int registration_provider = qmlRegisterUncreatableType<MinimapMarkerAttachedPropertyProvider>("yay.sfttech.openage", 1, 0, "MinimapMarker", "MinimapMarker is non-instantiable. It provides the 'MinimapMarker.type' attached property.");
const int registration_property = qmlRegisterType<MinimapMarkerAttachedProperty>();
}

gamedata::minimap_modes MinimapMarkerAttachedPropertyProvider::minimap_modes_to_gamedata(minimap_modes type) {
	return static_cast<gamedata::minimap_modes>(static_cast<std::underlying_type<decltype(type)>::type>(type));
}

MinimapMarkerAttachedPropertyProvider::minimap_modes MinimapMarkerAttachedPropertyProvider::minimap_modes_from_gamedata(gamedata::minimap_modes type) {
	return static_cast<minimap_modes>(static_cast<std::underlying_type<decltype(type)>::type>(type));
}

std::tuple<gamedata::minimap_modes, bool> MinimapMarkerAttachedPropertyProvider::int_to_minimap_modes_gamedata(int type) {
	auto &staticMetaObject = MinimapMarkerAttachedPropertyProvider::staticMetaObject;

	const char *enum_minimap_modes_name = "minimap_modes";
	int minimap_modes_enum_index = staticMetaObject.indexOfEnumerator(enum_minimap_modes_name);
	ENSURE(minimap_modes_enum_index != -1, "The " << enum_minimap_modes_name << " enum was not declared in " << staticMetaObject.className() << "class.");

	bool can_convert = staticMetaObject.enumerator(minimap_modes_enum_index).valueToKey(type);

	return std::make_tuple(can_convert ? static_cast<gamedata::minimap_modes>(type) : gamedata::minimap_modes(), can_convert);
}

MinimapMarkerAttachedProperty* MinimapMarkerAttachedPropertyProvider::qmlAttachedProperties(QObject *attachee) {
	return new MinimapMarkerAttachedProperty(attachee);
}

MinimapMarkerAttachedProperty::MinimapMarkerAttachedProperty(QObject *object)
	:
	QObject{object},
	type{} {
	Q_UNUSED(registration_provider);
	Q_UNUSED(registration_property);
}

MinimapMarkerAttachedPropertyProvider::minimap_modes MinimapMarkerAttachedProperty::get_type() const {
	return MinimapMarkerAttachedPropertyProvider::minimap_modes_from_gamedata(this->type);
}

void MinimapMarkerAttachedProperty::set_type(MinimapMarkerAttachedPropertyProvider::minimap_modes type) {
	this->type = MinimapMarkerAttachedPropertyProvider::minimap_modes_to_gamedata(type);
}

}} // namespace openage::gui
