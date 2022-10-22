// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>


namespace openage::renderer {

class WindowEventHandler : public QObject {
	Q_OBJECT

public:
	WindowEventHandler() = default;
	~WindowEventHandler() = default;

	/**
	 * When attached to a QWindow, we get its events in here.
	 */
	bool eventFilter(QObject *object, QEvent *event) override;

	/**
	 * Fetch all collected events and clear the queue.
	 */
	std::vector<std::shared_ptr<QEvent>> pop_events();

protected:
	std::vector<std::shared_ptr<QEvent>> event_queue;
};


} // namespace openage::renderer
