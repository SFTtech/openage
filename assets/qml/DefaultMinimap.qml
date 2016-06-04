// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtGraphicalEffects 1.0

import yay.sfttech.openage 1.0 as OA

OA.Minimap {
	id: root

	/**
	 * Size of the billboards used for markers.
	 */
	markerSize: metrics.unit

	/**
	 * Item to use as a camera view frame image.
	 */
	property Component viewFrame: Item {
		id: defaultViewFrame

		Repeater {
			id: shadowing

			model: ["black", "white"]

			Rectangle {
				x: (shadowing.count - index) * border.width / 2
				y: (shadowing.count - index) * border.width / 2
				width: defaultViewFrame.width - shadowing.count * border.width / 2
				height: defaultViewFrame.height - shadowing.count * border.width / 2

				color: "transparent"
				border.width: metrics.unit * 0.2
				border.color: modelData
			}
		}
	}

	markerAtlasSpacing: defaultMarkerAtlas.spacing
	markersTypes: defaultMarkerAtlas.markersTypes
	markerAtlas: defaultMarkerAtlas

	Item {
		width: defaultMarkerAtlas.width
		height: defaultMarkerAtlas.height

		MarkerAtlas {
			id: defaultMarkerAtlas

			markers: [
				Item {
					OA.MinimapMarker.type: OA.MinimapMarker.SQUARE_DOT

					width: defaultMarkerAtlas.width
					height: defaultMarkerAtlas.height

					Rectangle {
						anchors.centerIn: parent
						width: parent.width / 2
						height: parent.height / 2
					}
				},
				Item {
					id: diamond

					OA.MinimapMarker.type: OA.MinimapMarker.DIAMOND_DOT

					width: defaultMarkerAtlas.width
					height: defaultMarkerAtlas.height

					Rectangle {
						anchors.centerIn: parent
						width: parent.width * Math.sqrt(2) / 2
						height: parent.height * Math.sqrt(2) / 2
						transform: Rotation {
							angle: 45
							origin.x: diamond.width * Math.sqrt(2) / 4
							origin.y: diamond.height * Math.sqrt(2) / 4
						}
					}
				},
				Rectangle {
					OA.MinimapMarker.type: OA.MinimapMarker.LARGEDOT

					width: defaultMarkerAtlas.width
					height: defaultMarkerAtlas.height
				}
			]
		}

		/**
		 * Hack to hide the texture provider.
		 */
		layer.enabled: true
		layer.effect: OpacityMask {
			maskSource: Item {
			}
		}
	}

	Item {
		anchors.fill: parent

		Loader {
			x: root.viewFrameRect.x
			y: root.viewFrameRect.y
			width: root.viewFrameRect.width
			height: root.viewFrameRect.height

			sourceComponent: root.viewFrame
		}

		/**
		 * Clip to the diamond shape.
		 */
		layer.enabled: true
		layer.effect: OpacityMask {
			maskSource: Item {
				width: root.viewFrameRect.width
				height: root.viewFrameRect.height
				Rectangle {
					anchors.fill: parent
					transform: [
						Scale { xScale: root.viewFrameRect.height / root.viewFrameRect.width / Math.sqrt(2); yScale: 1 / Math.sqrt(2) },
						Rotation { angle: 45 },
						Scale { xScale: root.viewFrameRect.width / root.viewFrameRect.height },
						Translate { x: root.viewFrameRect.width / 2}
					]
				}
			}
		}
	}

	/**
	 * Metric propagation.
	 */
	FontMetrics {
		id: fontMetrics
	}

	property int metricsUnit: metrics ? metrics.unit : fontMetrics.averageCharacterWidth
}
