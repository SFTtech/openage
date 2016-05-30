// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

import yay.sfttech.openage 1.0

Item {
	id: root

	// input
	property EditorMode editorMode
	property GameSpec gameSpec
	property int iconHeight

	function toggle() {
		if (!categoryPicker.current && categoryButtons.children.length)
			categoryButtons.children[0].checked = true
		else
			for (var i = 0; i < categoryButtons.children.length; ++i) {
				var item = categoryButtons.children[i]

				if (item.exclusiveGroup && item.exclusiveGroup == categoryPicker && item.checked) {
					for (var j = (i + 1) % categoryButtons.children.length; j != i; j = (j + 1) % categoryButtons.children.length) {
						var candidate = categoryButtons.children[j]

						if (candidate.exclusiveGroup && candidate.exclusiveGroup == categoryPicker) {
							candidate.checked = true
							break
						}
					}

					break
				}
			}
	}

	// output
	readonly property int current: selectedType.current ? selectedType.current.thisTypeId : -1
	property int currentHighlighted: -1

	readonly property int currentTerrain: selectedTerrain.current ? selectedTerrain.current.thisTerrainId : -1
	readonly property bool paintTerrain: categoryPicker.current && categoryPicker.current.text == "terrain"

	ColumnLayout {
		anchors.fill: parent

		Row {
			id: categoryButtons

			anchors.horizontalCenter: implicitWidth < parent.width ? parent.horizontalCenter : undefined

			ExclusiveGroup {
				id: categoryPicker
			}

			Category {
				id: categoryToUse
				name: categoryPicker.current && categoryPicker.current.text != "terrain" ? categoryPicker.current.text : ""
				editorMode: root.editorMode
			}

			ButtonFlat {
				checkable: true
				exclusiveGroup: categoryPicker

				text: "terrain"
			}

			Repeater {
				model: editorMode.categories

				ButtonFlat {
					checkable: true
					exclusiveGroup: categoryPicker

					text: modelData
				}
			}
		}

		Rectangle {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.minimumWidth: metricsUnit * 10
			Layout.minimumHeight: metricsUnit * 10

			color: "transparent"

			border.width: 1
			border.color: "white"
			radius: 4

			Flickable {
				anchors.fill: parent
				anchors.margins: metricsUnit / 2

				boundsBehavior: Flickable.StopAtBounds

				clip: true
				contentWidth: width
				contentHeight: typeIcons.height

				Flow {
					id: typeIcons

					anchors.left: parent.left
					anchors.right: parent.right

					ExclusiveGroup {
						id: selectedType
					}

					Repeater {
						model: categoryPicker.current && categoryPicker.current.text != "terrain" ? categoryToUse : undefined

						ButtonFlat {
							checkable: true
							exclusiveGroup: selectedType

							property int thisTypeId: typeId

							property int buttonIconHeight: iconHeight
							iconSource: "image://by-graphic-id/" + display + ".0"

							onHoveredChanged: {
								if (hovered)
									root.currentHighlighted = typeId
								else if (root.currentHighlighted == typeId)
									root.currentHighlighted = -1
							}
						}
					}

					ExclusiveGroup {
						id: selectedTerrain
					}

					Repeater {
						model: categoryPicker.current && categoryPicker.current.text == "terrain" ? gameSpec.terrainIdCount : undefined

						ButtonFlat {
							checkable: true
							exclusiveGroup: selectedTerrain

							property int thisTerrainId: index

							property int buttonIconHeight: iconHeight
							iconSource: "image://by-terrain-id/" + index + ".0"
						}
					}
				}
			}
		}
	}

	/*
	 * Metric propagation.
	 */
	FontMetrics {
		id: fontMetrics
	}

	property int metricsUnit: metrics ? metrics.unit : fontMetrics.averageCharacterWidth
}
