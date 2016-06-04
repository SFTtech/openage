// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4

import yay.sfttech.openage 1.0 as OA

/**
 * Arranges given markers into a square grid and renders them to a texture.
 * The item->textureProvider()->texture() can be used to get it.
 */
Grid {
	id: root

	/**
	 * Items to render to the grid. Should be square.
	 */
	property list<Item> markers

	/**
	 * Width and height of the empty space to the left and top of each cell (in pixels).
	 */
	property int spacingDimension: 4

	/**
	 * Corresponding gamedata::minimap_mode that each marker represents.
	 * Must be an array of 'MinimapMarker.<value>' enum values.
	 */
	readonly property var markersTypes: d.markersTypes

	/**
	 * Size of a final texture in pixels.
	 */
	readonly property int textureDimension: Math.min(root.width, root.height)

	/**
	 * Width and height of the empty space to the left and top of each cell (in tex coords).
	 */
	readonly property real spacing: spacingDimension / textureDimension

	/**
	 * Default texture dimensions.
	 */
	width: 512
	height: 512

	QtObject {
		id: d
		property var markersTypes
	}

	onMarkersChanged: {
		d.markersTypes = []
		for (var i = 0; i < markers.length; ++i)
			d.markersTypes.push(markers[i].OA.MinimapMarker.type)
	}

	columns: Math.ceil(Math.sqrt(markers.length))

	layer.enabled: true

	Repeater {
		model: markers

		Item {
			width: root.width / root.columns
			height: root.height / root.columns

			ShaderEffectSource {
				anchors.fill: parent
				anchors.leftMargin: root.spacingDimension
				anchors.topMargin: root.spacingDimension

				sourceItem: modelData
			}
		}
	}
}
