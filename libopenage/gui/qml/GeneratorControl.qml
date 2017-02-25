// Copyright 2015-2017 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

import yay.sfttech.openage 1.0 as OA

Item {
	id: root

	property var generatorParameters
	property var gameSpec
	property var game

	implicitWidth: elements.width
	implicitHeight: elements.height

	Column {
		id: elements

		spacing: genActions.spacing

		ColumnLayout {
			id: genActions

			OA.GameSaver {
				id: gameSaver

				game: root.game
				generatorParameters: root.generatorParameters
			}

			ButtonFlat {
				Layout.fillWidth: true

				text: "save_game"
				onClicked: gameSaver.activate()
			}

			OA.GameCreator {
				id: gameCreator

				game: root.game
				gameSpec: root.gameSpec
				generatorParameters: root.generatorParameters
			}

			ButtonFlat {
				Layout.fillWidth: true

				text: "generate_game"
				onClicked: {
					gameCreator.activate()
					gameSaver.clearErrors()
				}
			}

			ButtonFlat {
				Layout.fillWidth: true

				text: "end_game"
				onClicked: {
					game.clear()
					gameCreator.clearErrors()
				}
			}

			ButtonFlat {
				Layout.fillWidth: true

				text: "reload_assets"
				onClicked: gameSpec.invalidate()
			}
		}

		Text {
			property string errorStringSeparator: gameCreator.errorString && gameSaver.errorString ? "\n" : ""
			property string errorString: gameCreator.errorString + errorStringSeparator + gameSaver.errorString

			color: errorString ? "red" : "white"

			text: if (errorString)
					"Error: " + errorString
				else
					switch (root.game.state) {
						case OA.GameMain.Null:
							"Not running"
							break

						case OA.GameMain.Running:
							"Running"
							break

						default:
							"Unknown"
							break
					}
		}
	}
}
