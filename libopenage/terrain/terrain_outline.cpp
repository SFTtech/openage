// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include <cmath>
#include <memory>

#include "../texture.h"
#include "terrain_outline.h"

namespace openage {

std::shared_ptr<Texture> square_outline(coord::tile_delta foundation_size) {
	int width = (foundation_size.ne + foundation_size.se) * 48;
	int height = (foundation_size.ne + foundation_size.se) * 24;

	auto image_data = std::make_unique<uint32_t[]>(width * height);
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			float w_percent = (float) abs(i - (width / 2)) / (float) (width / 2);
			float h_percent = (float) abs(j - (height / 2)) / (float) (height / 2);

			// draw line where (w_percent + h_percent) == 1
			// line variable is in range 0.0 to 1.0
			float line = 1.0f - fabs(1.0f - fabs(h_percent + w_percent));
			unsigned char inten = 255 * pow(line, 16 * width/96);
			image_data[i + j * width] = (inten << 24) | (inten << 16) | (inten << 8) | inten;
		}
	}

	return std::make_shared<Texture>(width, height, std::move(image_data));
}

std::shared_ptr<Texture> radial_outline(float radius) {
	// additional pixels around the edge
	int border = 4;

	// image size
	int width = border + radius * 96 * 2;
	int height = border + radius * 48 * 2;
	int half_width = width / 2;
	int half_height = height / 2;

	auto image_data = std::make_unique<uint32_t[]>(width * height);

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			float w_percent = (float) (border+i-half_width) / (float) (half_width-border);
			float h_percent = (float) (border/2+j-half_height) / (float) (half_height-border/2);

			// line drawn where distance to image center == 1
			// line variable is in range 0.0 to 1.0
			float line = 1.0f - fabs(1.0f - std::hypot(w_percent, h_percent));
			unsigned char inten = 255 * pow(line, 32 * width/96);
			image_data[i + j * width] = (inten << 24) | (inten << 16) | (inten << 8) | inten;
		}
	}

	return std::make_shared<Texture>(width, height, std::move(image_data));
}

} // namespace openage
