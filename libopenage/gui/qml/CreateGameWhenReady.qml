// Copyright 2015-2017 the openage authors. See copying.md for legal info.

import QtQuick 2.4

import yay.sfttech.openage 1.0 as OA

OA.GameCreator {
	id: root

	property bool enabled: false
	property var gameControl
	property int gameControlTargetModeIndex

	property int specState: gameSpec ? gameSpec.state : OA.GameSpec.Null
	property int gameState: game ? game.state : OA.GameMain.Null

	onSpecStateChanged: {
		if (enabled && specState == OA.GameSpec.Ready)
			activate()
	}

	onGameStateChanged: {
		if (enabled && gameState == OA.GameMain.Running)
			if (gameControl.modeIndex != -1)
				gameControl.modeIndex = gameControlTargetModeIndex
			else
				console.error("CreateGameWhenReady: could not find the desired mode to switch to")
	}
}
