// Copyright 2016-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4
import QtQuick.Controls 1.1

Grid {
	id: root

	/*
	 * List of actions. Fields:
	 * 'act' - button action of type 'Action' that may have 'url iconCheckedSource' property
	 *
	 * Use 'act.iconSource == ""' for gaps.
	 */
	property var buttonActions

	spacing: metricsUnit * 0.5

	Component {
		id: gridElement

		ButtonExtruded {
			property bool flat

			width: (root.height - root.spacing * (root.rows - 1)) / root.rows
			height: width

			visible: iconSource != ""

			property url iconCheckedSource
		}
	}

	Repeater {
		model: parent.buttonActions

		delegate: Loader {
			sourceComponent: gridElement
			active: typeof act !== "undefined"
			onLoaded: {
				item.iconSource = Qt.binding(function() { return act.iconSource })
				item.action = act

				item.flat = Qt.binding(function() { return act.iconSourceChecked != "" })
				item.iconCheckedSource = Qt.binding(function() { return act.iconCheckedSource })
			}
		}
	}

	/*
	 * Metric propagation.
	 */
	FontMetrics {
		id: fontMetrics
	}

	property int metricsUnit: metrics ? metrics.unit : fontMetrics.averageCharacterWidth
}
