// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_LABEL_H_
#define OPENAGE_GUI_LABEL_H_

#include "control.h"
#include "alignment.h"
#include "../engine.h"

namespace openage {
namespace gui {

class Label : public Control {
public:
	Label();

	void set_text(const std::string &text);

	void set_horizontal_alignment(HorizontalAlignment horizontal_alignment);

	void draw(const Drawer &drawer) const override;

	virtual std::tuple<int, int> get_best_size() const override {
		return std::make_tuple(100, default_height());
	}

	static constexpr int default_height() { return 20; }

protected:
	std::string text;

	HorizontalAlignment horizontal_alignment = HorizontalAlignment::LEFT;
};

} // namespace gui
} // namespace openage

#endif
