// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
	id: root

	/*
	 * A paper texture.
	 */
	property url paperTextureSource: "image://by-filename/converted/interface/scr5b.slp.png.0"

	/*
	 * This texture is tiled along the item edges, bottom faces the inside.
	 * So, alpha should be zero for the pixels where to keep the paper, solid color - where it's torn off.
	 */
	property url edgeHmapSource: "image://by-filename/textures/torn_paper_edge.png.0"

	/*
	 * Which edges are torn off.
	 */
	property bool tornTop: true
	property bool tornBottom: true
	property bool tornLeft: true
	property bool tornRight: true

	/*
	 * Changing metrics.unit (currently bound to the font) scales all textures.
	 * So the details on the textures stay exactly the same relatively to metricsUnit.
	 */
	property real scaling: 8. / metricsUnit

	transform: Scale { xScale: 1. / root.scaling; yScale: 1. / root.scaling}

	Item {
		anchors.left: parent.left
		anchors.top: parent.top

		/*
		 * Do +.5 to avoid being less than parent due to truncation.
		 */
		width: (parent.width + .5) * root.scaling
		height: (parent.height + .5) * root.scaling

		/**
		 * Tears the edges.
		 * TODO: use QtGraphicalEffects.OpacityMask from Qt 5.7.
		 */
		OpacityMask57 {
			id: tornPaper

			anchors.fill: parent

			source: Blend {
				width: tornPaper.width
				height: tornPaper.height

				source: Image {
					width: tornPaper.width
					height: tornPaper.height

					source: root.paperTextureSource
					fillMode: Image.Tile
				}

				/**
				 * Darken near the edges.
				 */
				foregroundSource: RadialGradient {
					width: tornPaper.width
					height: tornPaper.height

					verticalOffset: height / 4

					gradient: Gradient {
						GradientStop { position: 0.0; color: "#FFFFFFFF" }
						GradientStop { position: 0.5; color: "#FF999999" }
					}
				}

				mode: "colorBurn"
			}

			maskSource: rectWithTornOffEdges
			invert: true
		}

		/*
		 * Alpha == 1.0 where the edges are torn off.
		 */
		Item {
			id: rectWithTornOffEdges

			anchors.fill: parent
			layer.enabled: true

			Image {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top

				source: root.edgeHmapSource
				fillMode: Image.Tile

				visible: root.tornTop
			}

			Image {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top

				source: root.edgeHmapSource
				fillMode: Image.Tile

				transform: Rotation {
					origin.x: rectWithTornOffEdges.width / 2
					origin.y: rectWithTornOffEdges.height / 2
					angle: 180
				}

				visible: root.tornBottom
			}

			Image {
				anchors.left: parent.left
				anchors.leftMargin: parent.height
				anchors.top: parent.top

				width: parent.height

				source: root.edgeHmapSource
				fillMode: Image.Tile

				transform: Rotation {
					origin.y: rectWithTornOffEdges.height
					angle: -90
				}

				visible: root.tornLeft
			}

			Image {
				anchors.right: parent.right
				anchors.rightMargin: parent.height
				anchors.top: parent.top

				width: parent.height

				source: root.edgeHmapSource
				fillMode: Image.Tile

				transform: Rotation {
					origin.y: rectWithTornOffEdges.height
					origin.x: rectWithTornOffEdges.height
					angle: 90
				}

				visible: root.tornRight
			}

			visible: false
		}
	}

	/*
	 * Average character width that the paper will adapt to (by scaling its texture).
	 */
	/*
	 * Metric propagation.
	 */
	FontMetrics {
		id: fontMetrics
	}

	property int metricsUnit: metrics ? metrics.unit : fontMetrics.averageCharacterWidth
}
