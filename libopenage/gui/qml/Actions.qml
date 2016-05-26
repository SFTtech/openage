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
	 * Signal that actions are filled in.
	 */
	signal ready(Repeater actions)

	visible: false

	ExclusiveGroup {
		id: stanceGrp
	}

	ExclusiveGroup {
		id: noGrp
	}

	Repeater {
		id: actions

		model: ListModel {
			Component.onCompleted: {
				append({ico: 6,  icoChk: -1, grp: noGrp,     name: "SET_ABILITY_PATROL"})
				append({ico: 7,  icoChk: -1, grp: noGrp,     name: "SET_ABILITY_GUARD"})
				append({ico: 8,  icoChk: -1, grp: noGrp,     name: "SET_ABILITY_FOLLOW"})
				append({ico: 59, icoChk: -1, grp: noGrp,     name: "KILL_UNIT"})
				append({ico: 2,  icoChk: -1, grp: noGrp,     name: "SET_ABILITY_GARRISON"})

				append({ico: 9,  icoChk: 53, grp: stanceGrp, name: "AGGRESSIVE_STANCE"})
				append({ico: 10, icoChk: 52, grp: stanceGrp, name: "DEFENSIVE_STANCE"})
				append({ico: 11, icoChk: 51, grp: stanceGrp, name: "HOLD_STANCE"})
				append({ico: 50, icoChk: 54, grp: stanceGrp, name: "PASSIVE_STANCE"})
				append({ico: 3,  icoChk: -1, grp: noGrp,     name: "STOP"})

				root.ready(actions)
			}
		}

		delegate: Item {
			Action {
				onTriggered: root.actionMode.act(name)

				iconSource: ico != -1 ? root.iconsSource + "." + ico : ""

				property url iconCheckedSource: icoChk != -1 ? root.iconsSource + "." + icoChk : ""
				checkable: icoChk != -1
				exclusiveGroup: icoChk != -1 ? grp : null
			}
		}
	}
}
