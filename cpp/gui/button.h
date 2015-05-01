// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_BUTTON_H_
#define OPENAGE_GUI_BUTTON_H_

#include "container.h"
#include "forward.h"
#include "signal.h"

namespace openage {
namespace gui {

class Button : public Container {
public:
	enum class State {
		NORMAL, HOVERED, DOWN, DOWN_OUT
	};

	virtual void mouse_left_bg() override;
	virtual bool mouse_moved_bg(int x, int y) override;
	virtual bool mouse_pressed_bg(std::uint8_t button) override;
	virtual void mouse_released_bg(std::uint8_t button) override;

	void on_click(Signal<>::function_t &&handler) {
		sig_click.connect(std::move(handler));
	}

	void on_state_change(Signal<State>::function_t &&handler) {
		handler(get_state());
		sig_state_change.connect(std::move(handler));
	}

	State get_state() const {
		return state;
	}
protected:
	virtual void set_state(State new_state);

	State state = State::NORMAL;

	Signal<> sig_click;
	Signal<State> sig_state_change;
};

class LabelButton : public Button {
public:
	Label *get_label() {
		return label;
	}

	void set_label(Label *label) {
		this->label = label;
	}

protected:
	Label *label = nullptr;
};

} // namespace gui
} // namespace openage

#endif
