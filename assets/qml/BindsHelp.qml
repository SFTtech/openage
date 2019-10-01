// Copyright 2015-2017 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1

import yay.sfttech.openage 1.0 as OA

Column {
	property var gameControl

	Text {
		id: dummyText
		font.pointSize: 9
	}

	Text {
		font.pointSize: dummyText.font.pointSize + 3

		color: "white"
		text: gameControl.mode ? gameControl.mode.name : "No Mode"
	}

	Repeater {
		model: gameControlObj.mode ? gameControlObj.mode.binds : undefined

		Text {
			font.pointSize: dummyText.font.pointSize
			color: "white"
			text: modelData
		}
	}

	Text {
		font.pointSize: dummyText.font.pointSize + 3

		color: "white"
		text: "Global Bindings"
	}

	Repeater {
		model: OA.Engine.globalBinds

		Text {
			font.pointSize: dummyText.font.pointSize
			color: "white"
			text: modelData
		}
	}
}
