// Copyright 2015-2017 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3

import yay.sfttech.livereload 1.0
import yay.sfttech.openage 1.0 as OA

Item {
	id: root

	/*
	 * Global metric declaration.
	 */
	FontMetrics {
		id: fontMetrics
	}

	QtObject {
		id: metrics
		property real scale: 1
		property int unit: fontMetrics.averageCharacterWidth * scale
	}

	OA.GameSpec {
		id: specObj

		/**
		 * States: Null, Loading, Ready
		 */

		// Start loading assets immediately
		active: true

		assetManager: amObj

		LR.tag: "spec"
	}

	OA.AssetManager {
		id: amObj

		assetDir: OA.MainArgs.assetDir

		engine: OA.Engine

		LR.tag: "am"
	}

	OA.GameMain {
		id: gameObj

		/**
		 * States: Null, Running
		 */

		engine: OA.Engine

		LR.tag: "game"
	}

	OA.GameControl {
		id: gameControlObj

		engine: OA.Engine
		game: gameObj

		/**
		 * must be run after the engine is attached
		 */
		Component.onCompleted: {
			modes = [createModeObj, settingsModeObj, editorModeObj, actionModeObj]

			if (modeIndex == -1)
				modeIndex = 0
		}

		LR.tag: "gamecontrol"
	}

	OA.GeneratorParameters {
		id: genParamsObj

		LR.tag: "gen"
	}

	ColumnLayout {
		id: controls

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: fontMetrics.averageCharacterWidth * 2

		state: gameControlObj.mode ? gameControlObj.mode.name : ""

		Component {
			id: changeMode

			ButtonFlat {
				text: "change_mode"
				onClicked: gameControlObj.modeIndex = (gameControlObj.effectiveModeIndex + 1) % gameControlObj.modes.length
			}
		}

		OA.CreateMode {
			id: createModeObj
			LR.tag: "createMode"
		}

		OA.ActionMode {
			id: actionModeObj
			LR.tag: "actionMode"
		}

		OA.EditorMode {
			id: editorModeObj

			currentTypeId: typePicker.current
			currentTerrainId: typePicker.currentTerrain
			paintTerrain: typePicker.paintTerrain

			onToggle: typePicker.toggle()

			LR.tag: "editorMode"
		}

		SettingsMode {
			id: settingsModeObj
			LR.tag: "settingsMode"
		}

		CreateGameWhenReady {
			enabled: createWhenReady.checked

			game: gameObj
			gameSpec: specObj
			generatorParameters: genParamsObj
			gameControl: gameControlObj

			gameControlTargetModeIndex: gameControlObj.modes.indexOf(actionModeObj)
		}

		states: [
			State {
				id: creationMode
				name: createModeObj.name

				property list<Item> content: [
					Loader {
						sourceComponent: changeMode
					},
					GeneratorParametersConfiguration {
						generatorParameters: genParamsObj
					},
					GeneratorControl {
						generatorParameters: genParamsObj
						gameSpec: specObj
						game: gameObj
					},
					CheckBoxFlat {
						id: createWhenReady
						text: "create_when_ready"
						visible: specObj.state == OA.GameSpec.Loading
					}
				]

				PropertyChanges {
					target: controls
					children: creationMode.content
					anchors.topMargin: fontMetrics.averageCharacterWidth * 4
					anchors.leftMargin: fontMetrics.averageCharacterWidth * 2
				}
			},
			State {
				id: editorMode
				name: editorModeObj.name

				property list<Item> content: [
					Loader {
						sourceComponent: changeMode
					},
					TypePicker {
						id: typePicker

						Layout.preferredWidth: root.width / 4
						Layout.preferredHeight: root.height / 2

						iconHeight: fontMetrics.averageCharacterWidth * 8

						editorMode: editorModeObj
						gameSpec: specObj
					},
					Text {
						color: "white"
						text: typePicker.currentHighlighted != -1 ? typePicker.currentHighlighted : ""
					}
				]

				PropertyChanges {
					target: controls
					children: editorMode.content
					anchors.topMargin: fontMetrics.averageCharacterWidth * 4
					anchors.leftMargin: fontMetrics.averageCharacterWidth * 2
				}
			},
			State {
				id: actionMode
				name: actionModeObj.name

				property list<Item> content: [
					IngameHud {
						anchors.fill: root

						actionMode: actionModeObj
						playerName: gameControlObj.currentPlayerName
						civIndex: gameControlObj.currentCivIndex
					}
				]

				PropertyChanges {
					target: controls; children: actionMode.content
				}
			},
			State {
				id: settingsMode
				name: settingsModeObj.name

				property list<Item> content: [
					SettingsMenu {
						anchors.fill: root

						settingsMode: settingsModeObj
					}
				]

				PropertyChanges {
					target: controls; children: settingsMode.content
				}
			}
		]
	}

	ColumnLayout {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.topMargin: fontMetrics.averageCharacterWidth * 4
		anchors.rightMargin: fontMetrics.averageCharacterWidth * 2

		spacing: fontMetrics.averageCharacterWidth * 2

		BindsHelp {
			Layout.alignment: Qt.AlignRight

			gameControl: gameControlObj
		}
	}

	Component.onCompleted: {
		OA.ImageProviderByFilename.gameSpec = specObj
		OA.ImageProviderById.gameSpec = specObj
		OA.ImageProviderByTerrainId.gameSpec = specObj
	}
}
