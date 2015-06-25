// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "formlayout.h"
#include "container.h"

#include <cassert>

namespace openage {
namespace gui {

FormLayoutData FormLayoutData::best_size(const Control *control) {
	int best_w, best_h;
	std::tie(best_w, best_h) = control->get_best_size();

	return FormLayoutData()
			.set_left(FormAttachment::adjacent_to(control).offset_by(-best_w))
			.set_right(FormAttachment::adjacent_to(control).offset_by(best_w))
			.set_top(FormAttachment::adjacent_to(control).offset_by(-best_h))
			.set_bottom(FormAttachment::adjacent_to(control).offset_by(best_h));
}

void FormLayout::layout(const std::vector<std::unique_ptr<Control>> &controls, int container_w, int container_h) {
	if (last_width == container_w && last_height == container_h) {
		return;
	}

	last_width = container_w;
	last_height = container_h;

	for (auto &control : controls) {
		get_layout_data(control.get())->invalidate();
	}
	for (auto &control : controls) {
		layout(control.get(), container_w, container_h);
	}
}

void FormLayout::layout(const Control *control, int container_w, int container_h) {
	FormLayoutData *data = dynamic_cast<FormLayoutData *>(get_layout_data(control));
	assert(data->position[3] != FormLayoutData::EVALUATING_POS);
	if (data->position[3] == FormLayoutData::INVALID_POS) {
		data->position[3] = FormLayoutData::EVALUATING_POS;
	}
	else {
		return;
	}

	std::array<int, 2> naturalPos = {{container_w, container_h}};
	for (size_t i = 0; i < 4; ++i) {
		auto attach = data->attachments[i];

		if (attach.control == control) { // control attached to itself
			assert(attach.alignment == FormAttachment::Alignment::ADJACENT);
			auto &other = data->attachments[i ^ 1];
			attach.alignment = FormAttachment::Alignment::ALIGNED;
			attach.control = other.control;
			attach.offset += other.offset;
			attach.numerator = attach.numerator * other.denominator + other.numerator * attach.denominator;
			attach.denominator *= other.denominator;
		}

		int pos = naturalPos[i / 2] * attach.numerator / attach.denominator + attach.offset;
		if (attach.control) {
			layout(attach.control, container_w, container_h); // has to be a sibling!
			FormLayoutData *other_data = dynamic_cast<FormLayoutData *>(get_layout_data(attach.control));
			switch(attach.alignment) {
			case FormAttachment::Alignment::ADJACENT:
				pos += other_data->position[i ^ 1];
				break;
			case FormAttachment::Alignment::ALIGNED:
				pos += other_data->position[i];
				break;
			case FormAttachment::Alignment::CENTER:
				pos += (other_data->position[i] + other_data->position[i ^ 1]) / 2;
			}
		}
		data->position[i] = pos;
	}
}

std::tuple<int, int> FormLayout::get_best_size(const std::vector<std::unique_ptr<Control>> &controls) {
	std::tuple<int, int> retVal{0, 0};
	for(auto &ctrl : controls) {
		auto s = ctrl->get_best_size();
		std::get<0>(retVal) = std::max(std::get<0>(retVal), std::get<0>(s));
		std::get<1>(retVal) = std::max(std::get<1>(retVal), std::get<1>(s));
	}
	return retVal;
}

} // namespace gui
} // namespace openage
