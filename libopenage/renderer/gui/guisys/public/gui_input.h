// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QEvent>

#include "renderer/gui/guisys/private/gui_input_impl.h"

namespace qtgui {

class GuiRenderer;
class GuiInputImpl;

/**
 * Send QEvents from the main window system to the GUI subsystem.
 */
class GuiInput {
public:
	explicit GuiInput(const std::shared_ptr<GuiRenderer> &renderer);
	~GuiInput() = default;

	/**
	 * Returns true if the event was accepted.
	 */
	bool process(const std::shared_ptr<QEvent> &event);

private:
	friend class GuiInputImpl;
	std::unique_ptr<GuiInputImpl> impl;
};

} // namespace qtgui
