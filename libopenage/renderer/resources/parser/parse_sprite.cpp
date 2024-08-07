// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#include "parse_sprite.h"

#include <algorithm>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/animation/layer_info.h"
#include "renderer/resources/assets/cache.h"
#include "renderer/resources/parser/common.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/texture_info.h"
#include "util/file.h"
#include "util/path.h"
#include "util/strings.h"


namespace openage::renderer::resources::parser {

/**
 * Parse the layer attribute.
 *
 * @param args Arguments from the line with a \p layer attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
LayerData parse_layer(const std::vector<std::string> &args) {
	LayerData layer;

	layer.layer_id = std::stoul(args[1]);

	// Optional arguments
	auto keywordfuncs = std::unordered_map<std::string, std::function<void(std::vector<std::string>)>>{
		std::make_pair("mode", [&](const std::vector<std::string> &keywordargs) {
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
		std::make_pair("position", [&](const std::vector<std::string> &keywordargs) {
			layer.position = std::stoul(keywordargs[1]);
		}),
		std::make_pair("time_per_frame", [&](const std::vector<std::string> &keywordargs) {
			layer.time_per_frame = std::stof(keywordargs[1]);
		}),
		std::make_pair("replay_delay", [&](const std::vector<std::string> &keywordargs) {
			layer.replay_delay = std::stof(keywordargs[1]);
		})};

	for (size_t i = 2; i < args.size(); ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(keywordargs[0])) [[unlikely]] {
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
AngleData parse_angle(const std::vector<std::string> &args) {
	AngleData angle;

	angle.degree = std::stoul(args[1]);

	// Optional arguments
	auto keywordfuncs = std::unordered_map<std::string, std::function<void(std::vector<std::string>)>>{
		std::make_pair("mirror_from", [&](const std::vector<std::string> &keywordargs) {
			angle.mirror_from = std::stoul(keywordargs[1]);
		})};

	for (size_t i = 2; i < args.size(); ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(keywordargs[0])) [[unlikely]] {
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
FrameData parse_frame(const std::vector<std::string> &args) {
	FrameData frame;

	frame.index = std::stoul(args[1]);
	frame.angle = std::stoul(args[2]);
	frame.layer_id = std::stoul(args[3]);
	frame.texture_id = std::stoul(args[4]);
	frame.subtex_id = std::stoul(args[5]);

	return frame;
}

Animation2dInfo parse_sprite_file(const util::Path &path,
                                  const std::shared_ptr<AssetCache> &cache) {
	if (not path.is_file()) [[unlikely]] {
		throw Error(MSG(err) << "Reading .sprite file '"
		                     << path.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto file = path.open();
	auto lines = file.get_lines();

	float scalefactor = 1.0;
	std::vector<TextureData> textures;
	std::vector<LayerData> layers;
	std::vector<AngleData> angles;

	// largest frame index = total length of animation
	size_t largest_frame_idx = 0;

	// Map frame data to angle
	std::unordered_map<size_t, std::vector<FrameData>> frames;

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("version", [&](const std::vector<std::string> &args) {
			size_t version_no = parse_version(args);

			if (version_no != 2) {
				throw Error(MSG(err) << "Reading .sprite file '"
			                         << path.get_name()
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("texture", [&](const std::vector<std::string> &args) {
			textures.push_back(parse_texture(args));
		}),
		std::make_pair("scalefactor", [&](const std::vector<std::string> &args) {
			scalefactor = parse_scalefactor(args);
		}),
		std::make_pair("layer", [&](const std::vector<std::string> &args) {
			layers.push_back(parse_layer(args));
		}),
		std::make_pair("angle", [&](const std::vector<std::string> &args) {
			angles.push_back(parse_angle(args));
		}),
		std::make_pair("frame", [&](const std::vector<std::string> &args) {
			auto frame = parse_frame(args);
			if (frames.count(frame.angle) == 0) {
				std::vector<FrameData> angle_frames{};
				frames.emplace(std::make_pair(frame.angle, angle_frames));
			}
			frames.at(frame.angle).push_back(frame);

			// check for the largest index, so we can use it to
		    // interpolate the total animation length
			if (frame.index > largest_frame_idx) {
				largest_frame_idx = frame.index;
			}
		})};

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(args[0])) [[unlikely]] {
			throw Error(MSG(err) << "Reading .sprite file '"
			                     << path.get_name()
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}

		keywordfuncs[args[0]](args);
	}

	// Order frames by index
	for (auto angle_frames : frames) {
		std::sort(angle_frames.second.begin(),
		          angle_frames.second.end(),
		          [](FrameData &f1, FrameData &f2) {
					  return f1.index < f2.index;
				  });
	}

	// Order angles by degree
	std::sort(angles.begin(),
	          angles.end(),
	          [](AngleData &a1, AngleData &a2) {
				  return a1.degree < a2.degree;
			  });

	// Order layers by position
	std::sort(layers.begin(),
	          layers.end(),
	          [](LayerData &l1, LayerData &l2) {
				  return l1.position < l2.position;
			  });

	// Create ID map. Resolves IDs used in the file to array indices
	std::unordered_map<size_t, size_t> texture_id_map;
	for (size_t i = 0; i < textures.size(); ++i) {
		texture_id_map.insert(std::make_pair(textures.at(i).texture_id, i));
	}
	std::unordered_map<size_t, size_t> angle_id_map;
	for (size_t i = 0; i < angles.size(); ++i) {
		angle_id_map.insert(std::make_pair(angles.at(i).degree, i));
	}

	std::vector<LayerInfo> layer_infos;
	for (auto layer : layers) {
		std::vector<std::shared_ptr<AngleInfo>> angle_infos;

		// degree in the file is the angle center
		// we have to calculate the angle start
		// (between the angle center and the previous angle's center)
		float prev_angle_degree = angles.at(angles.size() - 1).degree;
		float angle_start = 0;

		for (auto angle : angles) {
			std::vector<std::shared_ptr<FrameInfo>> frame_infos;

			// get the degree where the angle starts
			if (prev_angle_degree > angle.degree) {
				// when previous angle > current angle, it wraps around at 360 degrees
				angle_start = prev_angle_degree + (360 + angle.degree - prev_angle_degree) / 2;
				if (angle_start >= 360) {
					// ensure that result is in 360 degrees range
					angle_start -= 360;
				}
			}
			else {
				angle_start = prev_angle_degree + (angle.degree - prev_angle_degree) / 2;
			}
			prev_angle_degree = angle.degree;

			if (angle.mirror_from >= 0) {
				// we can exit early if the angle is mirrored
				// because we don't need to store the frames
				auto angle_info_ptr = angle_infos.at(angle_id_map.at(angle.mirror_from));
				angle_infos.push_back(std::make_shared<AngleInfo>(angle_start,
				                                                  frame_infos,
				                                                  angle_info_ptr,
				                                                  flip_type::FLIP_X));
				continue;
			}

			for (auto frame : frames.at(angle.degree)) {
				if (frame.layer_id != layer.layer_id) {
					continue;
				}
				if (frame.index > frame_infos.size()) {
					// set empty frames if an index is missing
					for (size_t i = frame_infos.size() - 1; i < frame.index; ++i) {
						frame_infos.push_back(nullptr);
					}
				}
				frame_infos.push_back(std::make_shared<FrameInfo>(
					texture_id_map.at(frame.texture_id),
					frame.subtex_id));
			}
			if (frame_infos.size() < largest_frame_idx) {
				// insert empty frames at the end if an indices are missing
				for (size_t i = frame_infos.size() - 1; i < largest_frame_idx; ++i) {
					frame_infos.push_back(nullptr);
				}
			}

			angle_infos.push_back(std::make_shared<AngleInfo>(angle_start, frame_infos));
		}

		layer_infos.emplace_back(angle_infos,
		                         layer.mode,
		                         layer.position,
		                         layer.time_per_frame,
		                         layer.replay_delay);
	}

	// Parse textures
	std::vector<std::shared_ptr<Texture2dInfo>> texture_infos;
	for (auto texture : textures) {
		util::Path texturepath = (path.get_parent() / texture.path);

		if (cache && cache->check_texture_cache(texturepath)) {
			// already loaded
			texture_infos.push_back(cache->get_texture(texturepath));
		}
		else {
			// load (and cache if possible)
			auto info = std::make_shared<Texture2dInfo>(parse_texture_file(texturepath));
			texture_infos.push_back(info);
			if (cache) {
				cache->add_texture(texturepath, info);
			}
		}
	}

	return Animation2dInfo(scalefactor, texture_infos, layer_infos);
}

} // namespace openage::renderer::resources::parser
