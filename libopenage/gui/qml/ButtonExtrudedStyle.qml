// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3

/*
 * Additionally accepts 'bool flat' and 'url iconCheckedSource'.
 */
ButtonStyle {
	FontMetrics {
		id: fontMetrics
	}

	property int metricsUnit: metrics ? metrics.unit : fontMetrics.averageCharacterWidth

	background: Rectangle {
		anchors.fill: parent

		implicitWidth: 100
		implicitHeight: 25

		color: control.flat ? "transparent" : control.pressed ? "black" : "white"

		Image {
			anchors.fill: parent

			property int extrusion: control.flat ? 0 : metricsUnit / 4

			anchors.leftMargin: control.pressed ? 0 : extrusion
			anchors.topMargin: anchors.leftMargin
			anchors.rightMargin: control.pressed ? extrusion : 0
			anchors.bottomMargin: anchors.rightMargin

			source: control.checked ? control.iconCheckedSource : control.iconSource
			fillMode: Image.Stretch
		}
	}

	label: Item {
	}
}
