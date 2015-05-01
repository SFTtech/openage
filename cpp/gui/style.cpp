// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "style.h"

#include "border.h"
#include "button.h"
#include "image.h"
#include "label.h"
#include "gridlayout.h"

namespace openage {
namespace gui {

std::unique_ptr<LabelButton> Style::create_button() const {
#if 0
	auto button = std::make_unique<LabelButton>();
	auto border = button->create<gui::Border>();
	auto label = border->create<gui::Label>();

	auto texture = asset_manager->get_texture("gui/button.png");
	button->on_state_change([=](Button::State new_state){
		int add_offset = 0;

		switch(new_state) {
		case Button::State::DOWN:
			border->set_texture(texture, 9);
			add_offset = 1;
			break;
		default:
			border->set_texture(texture, 0);
		}

#if 0
		label->set_left(FormAttachment::left().offset_by(10 + add_offset));
		label->set_right(FormAttachment::right().offset_by(-10 + add_offset));
		label->set_bottom(FormAttachment::center().offset_by(10 - add_offset));
#endif
	});

	button->set_label(label);

	return button;
#endif
	return std::unique_ptr<LabelButton>();
}

std::unique_ptr<Button> Style::create_image_button(const char *texture, int base_subid) const {
	auto button = std::make_unique<Button>();
	button->set_layout(std::make_unique<GridLayout>());

	auto image = button->create<gui::Image>();
	image->set_layout_data(GridLayoutData::center());

	auto tex = asset_manager->get_texture(texture);
	button->on_state_change([=](Button::State new_state){
		switch(new_state) {
		case Button::State::DOWN:
			image->set_texture(tex, base_subid + 1);
			break;
		default:
			image->set_texture(tex, base_subid);
		}
	});

	return button;
}

} // namespace gui
} // namespace openage
