// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "parse_sprite.h"
#include "../../../error/error.h"
#include "../../../util/strings.h"

namespace openage::renderer::resources::parser {

/**
 * Parse the file version and check if the version is supported by
 * the parser.
 *
 * @param args Arguments from the line with a \p version attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return true if the version is supported, else false
 */
bool parse_version(std::vector<std::string> args) {
	return args[1] == "1";
}

/**
 * Parse the image attribute.
 *
 * @param args Arguments from the line with a \p image attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
ImageData parse_image(std::vector<std::string> args) {
	// TODO: Splitting at the space char assumes that the path string contains no
	// space. While the space char is not allowed because of nyan naming requirements,
	// it should result in an error if wrongly used here.
	ImageData image;

	image.id = std::stoul(args[1]);

	// Call substr() to get rid of the quotes
	image.path = args[2].substr(1, args[2].size() - 2);

	return image;
}

/**
 * Parse the scalefactor attribute.
 *
 * @param args Arguments from the line with a \p scalefactor attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Scalefactor value.
 */
float parse_scalefactor(std::vector<std::string> args) {
	return std::stof(args[1]);
}

/**
 * Parse the layer attribute.
 *
 * @param args Arguments from the line with a \p layer attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
LayerData parse_layer(std::vector<std::string> args) {
	LayerData layer;

	layer.id = std::stoul(args[1]);

	// Optional args
	for (size_t i = 2; i <= args.size(); ++i) {
		std::vector<std::string> keywordarg{util::split(args[i], '=')};

		if (keywordarg[0] == "mode") {
			if (keywordarg[1] == "off") {
				layer.mode = display_mode::OFF;
			}
			else if (keywordarg[1] == "once") {
				layer.mode = display_mode::ONCE;
			}
			else if (keywordarg[1] == "loop") {
				layer.mode = display_mode::LOOP;
			}
		}
		else if (keywordarg[0] == "position") {
			layer.position = std::stoul(keywordarg[1]);
		}
		else if (keywordarg[0] == "time_per_frame") {
			layer.time_per_frame = std::stof(keywordarg[1]);
		}
		else if (keywordarg[0] == "replay_delay") {
			layer.replay_delay = std::stof(keywordarg[1]);
		}
	}

	return layer;
}

/**
 * Parse the angle attribute.
 *
 * @param args Arguments from the line with a \p angle attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
AngleData parse_angle(std::vector<std::string> args) {
	AngleData angle;

	angle.degree = std::stoul(args[1]);

	for (size_t i = 2; i <= args.size(); ++i) {
		std::vector<std::string> keywordarg{util::split(args[i], '=')};

		if (keywordarg[0] == "mirror_from") {
			angle.mirror_from = std::stoul(keywordarg[1]);
		}
	}

	return angle;
}

/**
 * Parse the frame attribute.
 *
 * @param args Arguments from the line with a \p frame attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
FrameData parse_frame(std::vector<std::string> args) {
	FrameData frame;

	frame.index = std::stoul(args[1]);
	frame.angle = std::stoul(args[2]);
	frame.layer_id = std::stoul(args[3]);
	frame.image_id = std::stoul(args[4]);

	frame.xpos = std::stoul(args[5]);
	frame.ypos = std::stoul(args[6]);
	frame.xsize = std::stoul(args[7]);
	frame.ysize = std::stoul(args[8]);
	frame.xhotspot = std::stoul(args[9]);
	frame.yhotspot = std::stoul(args[10]);

	return frame;
}

Animation2d parse_sprite(util::Path &file) {
	auto content = file.open();
	auto lines = content.get_lines();

	std::vector<ImageData> images;
	std::vector<LayerData> layers;
	std::vector<AngleData> angles;
	std::vector<FrameData> frames;
	float scalefactor = 1.0;

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		if (args[0] == "version") {
			// TODO: Check support
		}
		else if (args[0] == "image") {
			images.push_back(parse_image(args));
		}
		else if (args[0] == "scalefactor") {
			scalefactor = parse_scalefactor(args);
		}
		else if (args[0] == "layer") {
			layers.push_back(parse_layer(args));
		}
		else if (args[0] == "angle") {
			angles.push_back(parse_angle(args));
		}
		else if (args[0] == "frame") {
			frames.push_back(parse_frame(args));
		}
	}
}

} // namespace openage::renderer::resources::parser