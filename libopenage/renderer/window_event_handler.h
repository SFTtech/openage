// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>


namespace openage::renderer {

/**
 * Filter events of a Qt window so they can be processed by the
 * openage renderer.
 */
class WindowEventHandler : public QObject {
	Q_OBJECT

public:
	/**
	 * Create a new event handler for filtering window events. After creation,
	 * it has to be attached to a QWindow with the installEventFilter() method.
	 *
	 */
	WindowEventHandler() = default;
	~WindowEventHandler() = default;

	/**
	 * Filter events of the window the filter is attached to and store them in the
	 * object's event queue.
	 *
	 * We filter these types of events:
	 *     - Key presses
	 *     - Mouse button presses
	 *     - Mouse moves
	 *     - Window resize
	 *     - Window close events
	 *
	 * @param object Window that received the event.
	 * @param event Received event.
	 *
	 * @return false if the event is forwarded to the window, true otherwise.
	 */
	bool eventFilter(QObject *object, QEvent *event) override;

	/**
	 * Fetch all collected events and clear the queue.
	 *
	 * @return List of filtered events.
	 */
	std::vector<std::shared_ptr<QEvent>> pop_events();

protected:
	/**
	 * Event queue for window events.
	 */
	std::vector<std::shared_ptr<QEvent>> event_queue;
};
} // namespace openage::renderer
