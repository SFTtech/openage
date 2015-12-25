// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.0

Item {
	Rectangle {
		id: r

		x: 0
		y: 0
		width: 100
		height: 100
		color: "green"

		MouseArea {
			anchors.fill: parent
			onClicked: r.color = "blue"
		}
	}
}
