// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cmath>
#include <ctime>

#include "heightmap.h"
#include "../../log/log.h"

namespace openage {
namespace mapgen {

Heightmap::Heightmap(coord::tile_delta size)
	:
	rng{(uint64_t)std::time(nullptr)} {

	// Get the biggest side, and round it up to 2^n + 1
	this->side_length = std::max(size.ne, size.se);
	this->side_length = std::exp2(std::ceil(std::log2(static_cast<float>(this->side_length)))) + 1;

	this->map.reserve(this->side_length * this->side_length);
}

std::vector<float> &Heightmap::generate() {
	const float roughness = 1.5;

	int dist = this->side_length - 1;
	float displacement = 1;

	// Initialize
	this->tile(0, 0) = 0;
	this->tile(dist, 0) = 0;
	this->tile(0, dist) = 0;
	this->tile(dist, dist) = 0;

	while (dist > 1) {
		log::log(MSG(dbg) << "dist = " << dist);
		int mid = dist / 2;

		// Diamond step: for each square, calculate the center from the corners
		// (x-mid,y-mid)  (x+mid,y-mid)
		//            \    /
		//            (x,y)
		//            /    \
		// (x-mid,y+mid)  (x+mid,y+mid)
		for (int y = mid; y < this->side_length; y += dist) {
			for (int x = mid; x < this->side_length; x += dist) {
				float diamond = 0;

				// Add corner values
				diamond += this->tile(x-mid, y-mid);
				diamond += this->tile(x+mid, y-mid);
				diamond += this->tile(x-mid, y+mid);
				diamond += this->tile(x+mid, y+mid);

				this->tile(x, y) = this->displace(diamond, 4, displacement);
				log::log(MSG(dbg) << "diamond (" << x << "," << y << ") -> " << this->tile(x, y));
			}
		}

		// Square step: for each diamond, calculate the center from the adjacent values
		//             (x,y-mid)
		//                |
		// (x-mid,y) -- (x,y) -- (x+mid,y)
		//                |
		//             (x,y+mid)

		// Handle tiles above and below diamond centers
		for (int y = 0; y < this->side_length; y += dist) {
			bool is_top = (y == 0);
			bool is_bottom = (y == this->side_length - 1);
			for (int x = mid; x < this->side_length; x += dist) {
				float square = 0;
				int neighbors = 2;

				// Add neighbor values
				if (!is_top) {
					square += this->tile(x, y - mid);
					neighbors++;
				}
				square += this->tile(x - mid, y);
				square += this->tile(x + mid, y);
				if (!is_bottom) {
					square += this->tile(x, y + mid);
					neighbors++;
				}

				this->tile(x, y) = this->displace(square, neighbors, displacement);
				log::log(MSG(dbg) << "square1 (" << x << "," << y << ") -> " << this->tile(x, y));
			}
		}

		// Handle tiles left and right of diamond centers
		for (int y = mid; y < this->side_length; y += dist) {
			for (int x = 0; x < this->side_length; x += dist) {
				bool is_left = (x == 0);
				bool is_right = (x == this->side_length - 1);

				float square = 0;
				int neighbors = 2;

				// Add neighbor values
				square += this->tile(x, y - mid);
				if (!is_left) {
					square += this->tile(x - mid, y);
					neighbors++;
				}
				if (!is_right) {
					square += this->tile(x + mid, y);
					neighbors++;
				}
				square += this->tile(x, y + mid);

				this->tile(x, y) = this->displace(square, neighbors, displacement);
				log::log(MSG(dbg) << "square2 (" << x << "," << y << ") -> " << this->tile(x, y));
			}
		}

		dist = mid;
		displacement *= std::pow(2.0, -roughness);
	}

	return this->map;
}

float &Heightmap::tile(int x, int y) {
	return this->map[this->side_length * y + x];
}

float Heightmap::displace(float value, int neighbors, float displacement) {
	// Calculate average + offset
	value /= neighbors;
	value += this->rng.real_range(-displacement, displacement);
	return value;
}

} // namespace mapgen
} // namespace openage
