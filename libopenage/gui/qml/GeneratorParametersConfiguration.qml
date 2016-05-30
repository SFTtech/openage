// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

import yay.sfttech.openage 1.0

Item {
	id: root

	property GeneratorParameters generatorParameters

	implicitHeight: parameterTable.height

	GridLayout {
		id: parameterTable

		Repeater {
			model: root.generatorParameters

			Text {
				Layout.row: index
				Layout.column: 0

				color: "white"

				text: display
			}
		}

		Repeater {
			model: root.generatorParameters

			Component {
				id: textField

				TextFieldFlat {
					property var model

					text: model.edit
					onTextChanged: model.edit = text
				}
			}

			Component {
				id: checkBox

				CheckBoxFlat {
					property var model

					checked: model.edit
					onCheckedChanged: model.edit = checked
				}
			}

			delegate: Loader {
				Layout.row: index
				Layout.column: 1

				sourceComponent: typeof(model.edit) == "boolean" ? checkBox : textField

				onLoaded: item.model = Qt.binding(function() { return model })
			}
		}
	}
}
