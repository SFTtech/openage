// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <QQuickWindow>

namespace openage::renderer {

class EventHandlingQuickWindow : public QQuickWindow {
	Q_OBJECT

public:
	EventHandlingQuickWindow();
	~EventHandlingQuickWindow() = default;

	/**
     * Get the input events for this window.
     * Clears the event queue afterwards.
     *
     */
	std::vector<QEvent *> poll_events();

protected:
	// Keyboard events
	virtual void keyPressEvent(QKeyEvent *ev) override;
	virtual void keyReleaseEvent(QKeyEvent *ev) override;

	// Mouse events
	virtual void mouseDoubleClickEvent(QMouseEvent *ev) override;
	virtual void mousePressEvent(QMouseEvent *ev) override;
	virtual void mouseReleaseEvent(QMouseEvent *ev) override;

	// Window resize events
	virtual void resizeEvent(QResizeEvent *ev) override;

private:
	// event queue
	std::vector<QEvent *> event_queue;
};

} // namespace openage::renderer