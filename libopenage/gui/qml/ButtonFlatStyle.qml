// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3

ButtonStyle {
	FontMetrics {
		id: fontMetrics
	}

	QtObject {
		id: d
		readonly property color color: control.hovered || control.checked ? "yellow" : "white"
	}

	background: Rectangle {
		implicitWidth: control.iconSource ? 0 : 100
		implicitHeight: control.iconSource ? 0 : 25

		color: "transparent"

		border.width:  control.text || control.hovered || !control.iconSource || control.checked ? 1 : 0
		border.color: control.checked ? "yellow" : "white"
		radius: 4
	}

	label: Item {
		implicitWidth: row.implicitWidth + (control.text ? fontMetrics.averageCharacterWidth * 2 : 0)
		implicitHeight: row.implicitHeight + (control.text ? fontMetrics.averageCharacterWidth : 0)

		Row {
			id: row
			anchors.centerIn: parent

			Image {
				sourceSize.width: control.buttonIconWidth
				sourceSize.height: control.buttonIconHeight
				source: control.iconSource
				anchors.verticalCenter: parent.verticalCenter
			}

			Text {
				id: text

				color: d.color

				text: control.text
			}
		}
	}
}
