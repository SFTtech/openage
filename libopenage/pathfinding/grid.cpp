// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "grid.h"


namespace openage::path {

CostGrid::CostGrid(size_t width, size_t height, size_t field_size) :
	width{width}, height{height}, fields{width * height, CostField{field_size}} {
}

std::pair<size_t, size_t> CostGrid::get_size() const {
	return {this->width, this->height};
}

CostField &CostGrid::get_field(size_t x, size_t y) {
	return this->fields[y * this->width + x];
}

} // namespace openage::path
