// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include <functional>

#include "../../../error/error.h"
#include "../../../util/strings.h"
#include "parse_sprite.h"
#include "parse_texture.h"

namespace openage::renderer::resources::parser {

/**
 * Parse the file version attribute.
 *
 * @param args Arguments from the line with a \p version attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Version number.
 */
size_t parse_spriteversion(std::vector<std::string> args) {
	return std::stoul(args[1]);
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

	// Optional arguments
	const auto keywordfuncs = datastructure::create_const_map<std::string, std::function<void(std::vector<std::string>)>>(
		std::make_pair("mode", [&](std::vector<std::string> keywordargs) {
			if (keywordargs[1] == "off") {
				layer.mode = display_mode::OFF;
			}
			else if (keywordargs[1] == "once") {
				layer.mode = display_mode::ONCE;
			}
			else if (keywordargs[1] == "loop") {
				layer.mode = display_mode::LOOP;
			}
		}),
		std::make_pair("position", [&](std::vector<std::string> keywordargs) {
			layer.position = std::stoul(keywordargs[1]);
		}),
		std::make_pair("time_per_frame", [&](std::vector<std::string> keywordargs) {
			layer.time_per_frame = std::stof(keywordargs[1]);
		}),
		std::make_pair("replay_delay", [&](std::vector<std::string> keywordargs) {
			layer.replay_delay = std::stof(keywordargs[1]);
		}));

	for (size_t i = 2; i < args.size(); ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		if (unlikely(!keywordfuncs.contains(keywordargs[0]))) {
			throw Error(MSG(err) << "Keyword argument "
			                     << keywordargs[0]
			                     << " of 'layer' attribute is not defined");
		}

		keywordfuncs[keywordargs[0]](keywordargs);
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

	// Optional arguments
	const auto keywordfuncs = datastructure::create_const_map<std::string, std::function<void(std::vector<std::string>)>>(
		std::make_pair("mirror_from", [&](std::vector<std::string> keywordargs) {
			angle.mirror_from = std::stoul(keywordargs[1]);
		}));

	for (size_t i = 2; i < args.size(); ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		if (unlikely(!keywordfuncs.contains(keywordargs[0]))) {
			throw Error(MSG(err) << "Keyword argument "
			                     << keywordargs[0]
			                     << " of 'angle' attribute is not defined");
		}

		keywordfuncs[keywordargs[0]](keywordargs);
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

	const auto keywordfuncs = datastructure::create_const_map<std::string, std::function<void(std::vector<std::string>)>>(
		std::make_pair("version", [&](std::vector<std::string> args) {
			size_t version_no = parse_spriteversion(args);

			if (version_no != 2) {
				throw Error(MSG(err) << "Reading .sprite file '"
			                         << file
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("texture", [&](std::vector<std::string> args) {
			textures.push_back(parse_texture(args));
		}),
		std::make_pair("scalefactor", [&](std::vector<std::string> args) {
			scalefactor = parse_scalefactor(args);
		}),
		std::make_pair("layer", [&](std::vector<std::string> args) {
			layers.push_back(parse_layer(args));
		}),
		std::make_pair("angle", [&](std::vector<std::string> args) {
			angles.push_back(parse_angle(args));
		}),
		std::make_pair("frame", [&](std::vector<std::string> args) {
			auto frame = parse_frame(args);
			if (frames.count(frame.angle) == 0) {
				std::vector<FrameData> angle_frames{};
				frames.emplace(std::make_pair(frame.angle, angle_frames));
			}
			frames.at(frame.angle).push_back(frame);
		}));

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		if (unlikely(!keywordfuncs.contains(args[0]))) {
			throw Error(MSG(err) << "Reading .sprite file '"
			                     << file
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}

		keywordfuncs[args[0]](args);
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
