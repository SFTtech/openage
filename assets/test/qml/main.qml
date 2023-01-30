// Copyright 2015-2017 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import yay.sfttech.livereload 1.0
import yay.sfttech.openage 1.0 as OA

Item {
	id: root

	Rectangle {
		width: 200
		height: 200
		color: "red"

		Rectangle {
			x: 100
			y: 100
			width: 100
			height: 100
			color: "blue"

			Rectangle {
				id: r
				width: 50
				height: 50
				color: "green"
			}
		}

		MouseArea {
			anchors.fill: parent
			onClicked: r.color = "yellow"
		}
	}
}
