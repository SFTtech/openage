// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "gridlayout.h"
#include "container.h"

#include <cassert>

namespace openage {
namespace gui {

void GridLayout::layout(const std::vector<std::unique_ptr<Control>> &controls, int container_w, int container_h) {
}

std::tuple<int, int> GridLayout::get_best_size(const std::vector<std::unique_ptr<Control>> &controls) {
	int widest_row = 0;
	int highest_col = 0;
	int rows = 0;
	int cols = 0;

	for (auto &ctrl :  controls) {
		auto *data = dynamic_cast<GridLayoutData *>(get_layout_data(ctrl.get()));
		
		int row = data->y + data->h;
		if (row > rows) {
			rows = row;
		}

		int col = data->x + data->w;
		if (col > cols) {
			cols = col;
		}
	}

	for (int row = 0; row < rows; ++row) {
		int row_w = 0;
		for (auto &ctrl : controls) {
			auto *data = dynamic_cast<GridLayoutData *>(get_layout_data(ctrl.get()));
			if(data->y > row) continue;
			if(data->y + data->h <= row) continue;
			row_w += std::get<0>(ctrl->get_best_size());
		}

		if(row_w > widest_row) {
			widest_row = row_w;
		}
	}

	for (int col = 0; col < cols; ++col) {
		int col_h = 0;
		for (auto &ctrl : controls) {
			auto *data = dynamic_cast<GridLayoutData *>(get_layout_data(ctrl.get()));
			if(data->x > col) continue;
			if(data->x + data->w <= col) continue;
			col_h += std::get<1>(ctrl->get_best_size());
		}

		if(col_h > highest_col) {
			highest_col = col_h;
		}
	}

	return std::make_tuple(widest_row, highest_col);
}

} // namespace gui
} // namespace openage
