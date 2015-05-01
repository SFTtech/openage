// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "container.h"

namespace openage {
namespace gui {

constexpr int LayoutData::INVALID_POS;
constexpr int LayoutData::EVALUATING_POS;

void Layout::layout(ContainerBase &container) {
	layout(container.get_controls(), container.width(), container.height());
}

LayoutData *Layout::get_layout_data(const Control *control) {
	return control->layout_data.get();
}

} // namespace gui
} // namespace openage
