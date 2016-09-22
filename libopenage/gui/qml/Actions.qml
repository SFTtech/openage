// Copyright 2016-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1

import yay.sfttech.openage 1.0

Item {
	id: root

	/*
	 * Calls 'actionMode.act(name)' to execute actions.
	 */
	property ActionMode actionMode

	/*
	 * Atlas with the icons.
	 */
	property url iconsSource

	/*
	 * Use "act" properties of these objects to access the Action.
	 * Action has additional "iconCheckedSource" property.
	 */
	property alias actionObjects: actionObjectList.children

	visible: false

	ExclusiveGroup {
		id: stanceGrp
	}

	ExclusiveGroup {
		id: noGrp
	}

	Item {
		id: actionObjectList

		Repeater {
			model: ActionsListModel {
				action_mode: root.actionMode
			}

			delegate: Item {
				property QtObject act: Action {
					onTriggered: root.actionMode.act(name)

					iconSource: ico != -1 ? root.iconsSource + "." + ico : ""

					property url iconCheckedSource: icoChk != -1 ? root.iconsSource + "." + icoChk : ""
					checkable: icoChk != -1
					exclusiveGroup: icoChk == -1 ? null : (grpID == 1) ? stanceGrp : noGrp
				}
			}
		}
	}
}
