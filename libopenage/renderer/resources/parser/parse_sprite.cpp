// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include <functional>

#include "../../../error/error.h"
#include "../../../util/strings.h"
#include "parse_sprite.h"
#include "parse_texture.h"

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
 * Parse the texture attribute.
 *
 * @param args Arguments from the line with a \p texture attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
TextureData parse_texture(std::vector<std::string> args) {
	// TODO: Splitting at the space char assumes that the path string contains no
	// space. While the space char is not allowed because of nyan naming requirements,
	// it should result in an error if wrongly used here.
	TextureData texture;

	texture.texture_id = std::stoul(args[1]);

	// Call substr() to get rid of the quotes
	texture.path = args[2].substr(1, args[2].size() - 2);

	return texture;
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

	layer.layer_id = std::stoul(args[1]);

	// Optional args
	for (size_t i = 2; i < args.size(); ++i) {
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

	for (size_t i = 2; i < args.size(); ++i) {
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
	frame.texture_id = std::stoul(args[4]);
	frame.subtex_id = std::stoul(args[5]);

	return frame;
}

Animation2dInfo parse_sprite_file(const util::Path &file) {
	auto content = file.open();
	auto lines = content.get_lines();

	float scalefactor = 1.0;
	std::vector<TextureData> textures;
	std::vector<LayerData> layers;
	std::vector<AngleData> angles;

	// Map frame data to angle
	std::unordered_map<size_t, std::vector<FrameData>> frames;

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		if (args[0] == "version") {
			// TODO: Check support
		}
		else if (args[0] == "texture") {
			textures.push_back(parse_texture(args));
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
			auto frame = parse_frame(args);
			if (frames.count(frame.angle) == 0) {
				std::vector<FrameData> angle_frames{};
				frames.emplace(std::make_pair(frame.angle, angle_frames));
			}
			frames.at(frame.angle).push_back(frame);
		}
	}

	// Order frames by index
	for (auto frame : frames) {
		std::sort(frame.second.begin(),
		          frame.second.end(),
		          [](FrameData &f1, FrameData &f2) {
					  return f1.index < f2.index;
				  });
	}

	// Create ID map. Resolves IDs used in the file to array indices
	std::unordered_map<size_t, size_t> texture_id_map;
	for (size_t i = 0; i < textures.size(); ++i) {
		texture_id_map.insert(std::make_pair(textures[i].texture_id, i));
	}
	std::unordered_map<size_t, size_t> angle_id_map;
	for (size_t i = 0; i < angles.size(); ++i) {
		angle_id_map.insert(std::make_pair(angles[i].degree, i));
	}

	std::vector<LayerInfo> layer_infos;
	for (auto layer : layers) {
		std::vector<AngleInfo> angle_infos;
		std::vector<std::shared_ptr<AngleInfo>> angle_info_ptrs;
		for (auto angle : angles) {
			std::vector<FrameInfo> frame_infos;
			if (angle.mirror_from < 0) {
				for (auto frame : frames[angle.degree]) {
					if (frame.layer_id != layer.layer_id) {
						continue;
					}
					if (frame.index > frame_infos.size()) {
						// TODO: Add empty frames if no frame exists for an index
						for (size_t i = frame_infos.size() - 1; i < frame.index; ++i) {
							// frame_infos.emplace_back();
						}
					}
					frame_infos.emplace_back(texture_id_map[frame.texture_id],
					                         frame.subtex_id);
				}
				angle_info_ptrs.emplace_back(std::make_shared<AngleInfo>(angle.degree, frame_infos));
				angle_infos.emplace_back(angle.degree, frame_infos);
			}
			else {
				auto angle_info_ptr = angle_info_ptrs[angle_id_map[angle.mirror_from]];
				angle_infos.emplace_back(angle.degree, frame_infos, angle_info_ptr);
			}
		}

		layer_infos.emplace_back(angle_infos,
		                         layer.mode,
		                         layer.position,
		                         layer.time_per_frame,
		                         layer.replay_delay);
	}

	// Parse textures
	// TODO: Check if texture is already loaded
	std::vector<Texture2dInfo> texture_infos;
	for (auto texture : textures) {
		util::Path texturepath = (file.get_parent() / texture.path);
		texture_infos.push_back(parse_texture_file(texturepath));
	}

	return Animation2dInfo(scalefactor, texture_infos, layer_infos);
}

} // namespace openage::renderer::resources::parser
