// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>

Q_FORWARD_DECLARE_OBJC_CLASS(QEvent);
Q_FORWARD_DECLARE_OBJC_CLASS(QQuickWindow);

namespace qtgui {

class GuiRenderer;

/**
 * Send QEvents from the main window system to the GUI subsystem.
 */
class GuiInputImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiInputImpl(const std::shared_ptr<GuiRenderer> &renderer);
	~GuiInputImpl() = default;

	/**
	 * Returns true if the event was accepted.
	 */
	bool process(const std::shared_ptr<QEvent> &event);

private:
	std::shared_ptr<QQuickWindow> window;
};

} // namespace qtgui
