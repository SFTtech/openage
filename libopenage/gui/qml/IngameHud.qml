// Copyright 2015-2017 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

import yay.sfttech.openage 1.0 as OA

Item {
	id: root

	property var actionMode
	property string playerName
	property int civIndex

	readonly property int topStripSubid: 0
	readonly property int midStripSubid: 1
	readonly property int leftRectSubid: 2
	readonly property int rightRectSubid: 3
	readonly property int resBaseSubid: 4

	readonly property string srcPrefix: "image://by-filename/converted/interface/hud"
	readonly property string pad: "0000"
	readonly property string srcSuffix: ".slp.png"
	property string hudImageSource: srcPrefix + (pad + civIndex).slice(-pad.length) + srcSuffix

	width: 1289
	height: 960

	Item {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		height: metricsUnit * 1.5 * 2.5

		Image {
			anchors.fill: parent
			source: hudImageSource + "." + root.topStripSubid

			sourceSize.height: parent.height
			fillMode: Image.Tile
		}

		RowLayout {
			anchors.fill: parent
			anchors.leftMargin: metricsUnit
			anchors.rightMargin: metricsUnit
			anchors.verticalCenter: parent.verticalCenter
			spacing: metricsUnit * 0.7

			Row {
				spacing: metricsUnit * 0.7

				Component {
					id: resourceIndicator

					Rectangle {
						property string amount
						property int iconIndex

						width: metricsUnit * 1.5 * 6.5
						height: metricsUnit * 1.5 * 1.7

						color: "#7FFFFFFF"

						Rectangle {
							anchors.fill: parent
							anchors.rightMargin: metricsUnit * 0.3
							anchors.bottomMargin: metricsUnit * 0.3

							color: "black"

							Image {
								sourceSize.height: parent.height

								source: hudImageSource + "." + (root.resBaseSubid + iconIndex)
								fillMode: Image.PreserveAspectFit
							}

							Text {
								anchors.right: parent.right
								anchors.rightMargin: metricsUnit / 2
								anchors.verticalCenter: parent.verticalCenter
								text: amount

								color: "white"
							}
						}
					}
				}

				Repeater {
					model: OA.Resources {
						actionMode: root.actionMode
					}

					delegate: Loader {
						sourceComponent: resourceIndicator

						onLoaded: {
							item.amount = Qt.binding(function() { return display })
							item.iconIndex = Qt.binding(function() { return index })
						}
					}
				}

				Loader {
					sourceComponent: resourceIndicator

					onLoaded: {
						item.amount = "13/42"
						item.iconIndex = 4
					}
				}
			}

			Item {
				Layout.fillWidth: true
				Layout.minimumWidth: epoch.implicitWidth

				Text {
					id: epoch
					anchors.centerIn: parent
					color: "white"
					text: root.playerName + (actionMode.ability.length ? (" (" + actionMode.ability + ")") : "")
				}
			}

			Repeater {
				model: 5

				Rectangle {
					width: metricsUnit * 1.5 * 5
					height: metricsUnit * 1.5 * 1.5

					color: "transparent"
					border.width: 1
					border.color: "white"
				}
			}
		}
	}

	Item {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		height: metricsUnit * 1.5 * 16

		Image {
			id: leftRect

			anchors.left: parent.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			width: height * 1.63

			source: hudImageSource + "." + root.leftRectSubid
			fillMode: Image.Stretch

			ActionsGrid {
				id: actionsGrid

				anchors.fill: parent
				anchors.topMargin: parent.height * 40 / 218
				anchors.leftMargin: parent.height * 40 / 218 + metricsUnit * 1.4
				anchors.rightMargin: parent.height * 40 / 218 + metricsUnit * 0.5
				anchors.bottomMargin: parent.height * 40 / 218 - metricsUnit * 1.4

				columns: 5
				rows: 3

				Actions {
					id: actions

					actionMode: root.actionMode
				}

				buttonActions: actions.actionObjects
			}
		}

		ColumnLayout {
			anchors.left: leftRect.right
			anchors.right: rightRect.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			spacing: 0

			Image {
				id: borderStrip

				Layout.fillWidth: true

				sourceSize.height: metricsUnit * 1.4

				source: hudImageSource + "." + root.midStripSubid
				fillMode: Image.Tile
			}

			Rectangle {
				Layout.fillHeight: true
				Layout.fillWidth: true

				color: "#41110d"

				Paper {
					anchors.fill: parent
					tornBottom: false
				}
			}
		}

		Image {
			id: rightRect

			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			width: height * 2.01

			source: hudImageSource + "." + root.rightRectSubid
			fillMode: Image.Stretch
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
