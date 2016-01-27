// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3

CheckBoxStyle {
	FontMetrics {
		id: fontMetrics
	}

	QtObject {
		id: metrics
		readonly property int unit: fontMetrics.averageCharacterWidth
	}

	QtObject {
		id: d
		readonly property color color: control.hovered ? "yellow" : "white"
	}

	indicator: Rectangle {
		implicitWidth: metrics.unit * 2
		implicitHeight: metrics.unit * 2

		color: "transparent"

		border.width: 1
		border.color: "white"
		radius: 4

		Rectangle {
			visible: control.checked
			color: d.color
			radius: 1
			anchors.margins: metrics.unit * .5
			anchors.fill: parent
		}
	}

	label: Text {
		color: d.color
		text: control.text
	}
}
