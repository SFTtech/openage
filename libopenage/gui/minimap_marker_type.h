// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <type_traits>
#include <limits>
#include <tuple>

#include <QtQml>

#include "../gamedata/unit.gen.h"

namespace openage {
namespace gui {

class MinimapMarkerAttachedProperty;

/**
 * Non-instantiable type that is needed to use a 'MinimapMarker.type' attached property.
 */
class MinimapMarkerAttachedPropertyProvider : public QObject {
	Q_OBJECT

	Q_ENUMS(minimap_modes)

public:
	/**
	 * Feel free to implement the global enums binding in Qt.
	 */
	using underlying = std::underlying_type<gamedata::minimap_modes>::type;
	enum class minimap_modes: underlying {
		NO_DOT_0 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_0),
		SQUARE_DOT = static_cast<underlying>(gamedata::minimap_modes::SQUARE_DOT),
		DIAMOND_DOT = static_cast<underlying>(gamedata::minimap_modes::DIAMOND_DOT),
		DIAMOND_DOT_KEEPCOLOR = static_cast<underlying>(gamedata::minimap_modes::DIAMOND_DOT_KEEPCOLOR),
		LARGEDOT = static_cast<underlying>(gamedata::minimap_modes::LARGEDOT),
		NO_DOT_5 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_5),
		NO_DOT_6 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_6),
		NO_DOT_7 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_7),
		NO_DOT_8 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_8),
		NO_DOT_9 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_9),
		NO_DOT_10 = static_cast<underlying>(gamedata::minimap_modes::NO_DOT_10)
	};

	static gamedata::minimap_modes minimap_modes_to_gamedata(minimap_modes type);
	static minimap_modes minimap_modes_from_gamedata(gamedata::minimap_modes type);
	static std::tuple<gamedata::minimap_modes, bool> int_to_minimap_modes_gamedata(int type);

	static MinimapMarkerAttachedProperty* qmlAttachedProperties(QObject*);
};

/**
 * The 'MinimapMarker.type' attached property.
 */
class MinimapMarkerAttachedProperty : public QObject {
	Q_OBJECT

	Q_PROPERTY(openage::gui::MinimapMarkerAttachedPropertyProvider::minimap_modes type READ get_type WRITE set_type)

public:
	explicit MinimapMarkerAttachedProperty(QObject *object);

	MinimapMarkerAttachedPropertyProvider::minimap_modes get_type() const;
	void set_type(MinimapMarkerAttachedPropertyProvider::minimap_modes type);

private:
	/**
	 * Means that the item with this attached property is a texture for a corresponding gamedata::minimap_modes.
	 */
	gamedata::minimap_modes type;
};

}} // namespace openage::gui

QML_DECLARE_TYPEINFO(openage::gui::MinimapMarkerAttachedPropertyProvider, QML_HAS_ATTACHED_PROPERTIES)
Q_DECLARE_METATYPE(openage::gui::MinimapMarkerAttachedPropertyProvider::minimap_modes)
