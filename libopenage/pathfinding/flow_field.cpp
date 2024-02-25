// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "flow_field.h"

#include "error/error.h"
#include "log/log.h"

#include "pathfinding/integration_field.h"


namespace openage::path {

FlowField::FlowField(size_t size) :
	size{size},
	cells(this->size * this->size, 0) {
	log::log(DBG << "Created flow field with size " << this->size << "x" << this->size);
}

FlowField::FlowField(const std::shared_ptr<IntegrationField> &integrate_field) :
	size{integrate_field->get_size()},
	cells(this->size * this->size, 0) {
	this->build(integrate_field);
}

size_t FlowField::get_size() const {
	return this->size;
}

flow_t FlowField::get_cell(size_t x, size_t y) const {
	return this->cells.at(x + y * this->size);
}

flow_dir_t FlowField::get_dir(size_t x, size_t y) const {
	return static_cast<flow_dir_t>(this->get_cell(x, y) & FLOW_DIR_MASK);
}

void FlowField::build(const std::shared_ptr<IntegrationField> &integrate_field) {
	ENSURE(integrate_field->get_size() == this->get_size(),
	       "integration field size "
	           << integrate_field->get_size() << "x" << integrate_field->get_size()
	           << " does not match flow field size "
	           << this->get_size() << "x" << this->get_size());

	// Reset the flow field.
	this->reset();

	auto &integrate_cells = integrate_field->get_cells();
	auto &flow_cells = this->cells;

	for (size_t y = 0; y < this->size; ++y) {
		for (size_t x = 0; x < this->size; ++x) {
			size_t idx = y * this->size + x;

			if (integrate_cells[idx] == INTEGRATE_UNREACHABLE) {
				// Cell cannot be used as path
				continue;
			}

			// Find the neighbor with the smallest cost.
			flow_dir_t direction;
			integrate_t smallest_cost = INTEGRATE_UNREACHABLE;
			if (y > 0) {
				integrate_t cost = integrate_cells[idx - this->size];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH;
				}
			}
			if (x < this->size - 1 && y > 0) {
				integrate_t cost = integrate_cells[idx - this->size + 1];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH_EAST;
				}
			}
			if (x < this->size - 1) {
				integrate_t cost = integrate_cells[idx + 1];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::EAST;
				}
			}
			if (x < this->size - 1 && y < this->size - 1) {
				integrate_t cost = integrate_cells[idx + this->size + 1];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH_EAST;
				}
			}
			if (y < this->size - 1) {
				integrate_t cost = integrate_cells[idx + this->size];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH;
				}
			}
			if (x > 0 && y < this->size - 1) {
				integrate_t cost = integrate_cells[idx + this->size - 1];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::SOUTH_WEST;
				}
			}
			if (x > 0) {
				integrate_t cost = integrate_cells[idx - 1];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::WEST;
				}
			}
			if (x > 0 && y > 0) {
				integrate_t cost = integrate_cells[idx - this->size - 1];
				if (cost < smallest_cost) {
					smallest_cost = cost;
					direction = flow_dir_t::NORTH_WEST;
				}
			}

			// Set the flow field cell to pathable.
			flow_cells[idx] = flow_cells[idx] | FLOW_PATHABLE;

			// Set the flow field cell to the direction of the smallest cost.
			flow_cells[idx] = flow_cells[idx] | static_cast<uint8_t>(direction);
		}
	}
}

const std::vector<flow_t> &FlowField::get_cells() const {
	return this->cells;
}

void FlowField::reset() {
	for (auto &cell : this->cells) {
		cell = 0;
	}

	log::log(DBG << "Flow field has been reset");
}

} // namespace openage::path
