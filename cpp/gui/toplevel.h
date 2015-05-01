// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_TOPLEVEL_H_
#define OPENAGE_GUI_TOPLEVEL_H_

#include "absolutelayout.h"
#include "container.h"
#include "layout.h"

#include <SDL2/SDL_events.h>

namespace openage {
namespace gui {

class TopLevel : public Container {
public:
	TopLevel() {
		set_layout_data(AbsoluteLayoutData{});
	}

	void resize(int w, int h) {
		dynamic_cast<AbsoluteLayoutData &>(*layout_data).set_position(0, 0, w, h);
		needs_layout = true;
		layout->layout(*this);
	}

	virtual std::tuple<int, int> get_best_size() const override {
		return std::make_tuple(0, 0);
	}

	bool process_event(SDL_Event *event);

protected:
	std::unique_ptr<Control> control;
};

} // namespace gui
} // namespace openage

#endif

