// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_FORMLAYOUT_H_
#define OPENAGE_GUI_FORMLAYOUT_H_

#include "forward.h"
#include "layout.h"

#include <array>

namespace openage {
namespace gui {

/// Modelled after SWT.
class FormAttachment {
	friend class FormLayout;
public:
	static constexpr FormAttachment adjacent_to(const Control *control) {
		return {control, Alignment::ADJACENT, 0, 1};
	}

	static constexpr FormAttachment aligned_with(const Control *control) {
		return {control, Alignment::ALIGNED, 0, 1};
	}

	static constexpr FormAttachment centered_on(const Control *control) {
		return {control, Alignment::CENTER, 0, 1};
	}

	static constexpr FormAttachment fraction_parent(int numerator, int denominator) {
		return {nullptr, Alignment::ADJACENT, numerator, denominator};
	}

	static constexpr FormAttachment percent_parent(int percent) {
		return fraction_parent(percent, 100);
	}

	static constexpr FormAttachment left() {
		return fraction_parent(0, 1);
	}

	static constexpr FormAttachment center() {
		return fraction_parent(1, 2);
	}

	static constexpr FormAttachment right() {
		return fraction_parent(1, 1);
	}

	static constexpr FormAttachment top() {
		return left();
	}

	static constexpr FormAttachment bottom() {
		return right();
	}

	FormAttachment offset_by(int offset) const {
		FormAttachment retVal = *this;
		retVal.offset += offset;
		return retVal;
	}

protected:
	enum class Alignment {
		ADJACENT, // SWT.DEFAULT
		ALIGNED,
		CENTER
	};

	constexpr FormAttachment(const Control *control, Alignment alignment, int numerator, int denominator) :
			control(control),
			alignment(alignment),
			numerator(numerator),
			denominator(denominator),
			offset(0) {
		// empty
	}

	const Control *control;
	Alignment alignment;
	int numerator;
	int denominator;
	int offset;
};

class FormLayoutData : public LayoutData {
	friend class FormLayout;
public:
	static FormLayoutData fill() {
		return FormLayoutData{};
	}

	static FormLayoutData best_size(const Control *control);

	FormLayoutData set_left(const FormAttachment &left) {
		attachments[0] = left;
		return *this;
	}

	FormLayoutData set_right(const FormAttachment &right) {
		attachments[1] = right;
		return *this;
	}
	
	FormLayoutData set_top(const FormAttachment &top) {
		attachments[2] = top;
		return *this;
	}

	FormLayoutData set_bottom(const FormAttachment &bottom) {
		attachments[3] = bottom;
		return *this;
	}

protected:
	FormLayoutData() = default;

	// left, right, top, bottom (default: fill container)
	std::array<FormAttachment, 4> attachments = {{
		FormAttachment::left(),
		FormAttachment::right(),
		FormAttachment::top(),
		FormAttachment::bottom()
	}};
};

class FormLayout : public Layout {
public:
	virtual void layout(const std::vector<std::unique_ptr<Control>> &controls, int container_w, int container_h) override;
	
	virtual std::tuple<int, int> get_best_size(const std::vector<std::unique_ptr<Control>> &controls) override;
protected:
	virtual void layout(const Control *control, int container_w, int container_h);
};

} // namespace gui
} // namespace openage

#endif
