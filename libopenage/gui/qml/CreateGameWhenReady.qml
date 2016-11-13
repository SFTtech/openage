// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import yay.sfttech.openage 1.0

GameCreator {
	id: root

	property bool enabled: false
	property GameControl gameControl
	property int gameControlTargetModeIndex

	property int specState: gameSpec ? gameSpec.state : GameSpec.Null
	property int gameState: game ? game.state : GameMain.Null

	onSpecStateChanged: {
		if (enabled && specState == GameSpec.Ready)
			activate()
	}

	onGameStateChanged: {
		if (enabled && gameState == GameMain.Running)
			if (gameControl.modeIndex != -1)
				gameControl.modeIndex = gameControlTargetModeIndex
			else
				console.error("CreateGameWhenReady: could not find the desired mode to switch to")
	}
}
