// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "parse_terrain.h"

#include <functional>

#include "error/error.h"
#include "renderer/resources/assets/cache.h"
#include "renderer/resources/parser/common.h"
#include "renderer/resources/parser/parse_blendtable.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/terrain/frame_info.h"
#include "renderer/resources/terrain/layer_info.h"
#include "renderer/resources/texture_data.h"
#include "renderer/texture.h"
#include "util/strings.h"

namespace openage::renderer::resources::parser {

/**
 * Parse the blendtable attribute.
 *
 * @param args Arguments from the line with a \p blendtable attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
BlendtableData parse_blendtable(const std::vector<std::string> &args) {
	// TODO: Splitting at the space char assumes that the path string contains no
	// space. While the space char is not allowed because of nyan naming requirements,
	// it should result in an error if wrongly used here.
	BlendtableData blendtable;

	blendtable.table_id = std::stoul(args[1]);

	// Call substr() to get rid of the quotes
	blendtable.path = args[2].substr(1, args[2].size() - 2);

	return blendtable;
}

/**
 * Parse the layer attribute.
 *
 * @param args Arguments from the line with a \p layer attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
TerrainLayerData parse_terrain_layer(const std::vector<std::string> &args) {
	TerrainLayerData layer;

	layer.layer_id = std::stoul(args[1]);

	// Optional arguments
	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("mode", [&](const std::vector<std::string> &keywordargs) {
			if (keywordargs[1] == "off") {
				layer.mode = terrain_display_mode::OFF;
			}
			else if (keywordargs[1] == "loop") {
				layer.mode = terrain_display_mode::LOOP;
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
 * Parse the frame attribute.
 *
 * @param args Arguments from the line with a \p frame attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
TerrainFrameData parse_terrain_frame(const std::vector<std::string> &args) {
	TerrainFrameData frame;

	frame.index = std::stoul(args[1]);
	frame.layer_id = std::stoul(args[2]);
	frame.texture_id = std::stoul(args[3]);
	frame.subtex_id = std::stoul(args[4]);

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("priority", [&](const std::vector<std::string> &keywordargs) {
			frame.priority = std::stoul(keywordargs[1]);
		}),
		std::make_pair("blend_mode", [&](const std::vector<std::string> &keywordargs) {
			frame.blend_mode = std::stoul(keywordargs[1]);
		})};

	for (size_t i = 4; i < args.size() - 1; ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		if (unlikely(!keywordfuncs.contains(keywordargs[0]))) {
			throw Error(MSG(err) << "Keyword argument "
			                     << keywordargs[0]
			                     << " of 'frame' attribute is not defined");
		}

		keywordfuncs[keywordargs[0]](keywordargs);
	}

	return frame;
}

TerrainInfo parse_terrain_file(const util::Path &file,
                               const std::shared_ptr<AssetCache> &cache) {
	if (unlikely(!file.is_file())) {
		throw Error(MSG(err) << "Reading .terrain file '"
		                     << file.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto content = file.open();
	auto lines = content.get_lines();

	float scalefactor = 1.0;
	std::vector<TextureData> textures;
	std::optional<BlendtableData> blendtable;
	std::vector<TerrainLayerData> layers;

	// largest frame index = total length of animation
	size_t largest_frame_idx = 0;

	// Map frame data to layer
	std::unordered_map<size_t, std::vector<TerrainFrameData>> frames;

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("version", [&](const std::vector<std::string> &args) {
			size_t version_no = parse_version(args);

			if (version_no != 2) {
				throw Error(MSG(err) << "Reading .terrain file '"
			                         << file.get_name()
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("texture", [&](const std::vector<std::string> &args) {
			textures.push_back(parse_texture(args));
		}),
		std::make_pair("blendtable", [&](const std::vector<std::string> &args) {
			blendtable = parse_blendtable(args);
		}),
		std::make_pair("scalefactor", [&](const std::vector<std::string> &args) {
			scalefactor = parse_scalefactor(args);
		}),
		std::make_pair("layer", [&](const std::vector<std::string> &args) {
			layers.push_back(parse_terrain_layer(args));
		}),
		std::make_pair("frame", [&](const std::vector<std::string> &args) {
			auto frame = parse_terrain_frame(args);
			if (frames.count(frame.layer_id) == 0) {
				std::vector<TerrainFrameData> layer_frames{};
				frames.emplace(std::make_pair(frame.layer_id, layer_frames));
			}
			frames.at(frame.layer_id).push_back(frame);

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

		if (unlikely(!keywordfuncs.contains(args[0]))) {
			throw Error(MSG(err) << "Reading .terrain file '"
			                     << file.get_name()
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}

		keywordfuncs[args[0]](args);
	}

	// Order frames by index
	for (auto angle_frames : frames) {
		std::sort(angle_frames.second.begin(),
		          angle_frames.second.end(),
		          [](TerrainFrameData &f1, TerrainFrameData &f2) {
					  return f1.index < f2.index;
				  });
	}

	// Create ID map. Resolves IDs used in the file to array indices
	std::unordered_map<size_t, size_t> texture_id_map;
	for (size_t i = 0; i < textures.size(); ++i) {
		texture_id_map.insert(std::make_pair(textures[i].texture_id, i));
	}

	std::vector<TerrainLayerInfo> layer_infos;
	for (auto layer : layers) {
		std::vector<std::shared_ptr<TerrainFrameInfo>> frame_infos;
		for (auto frame : frames[layer.layer_id]) {
			if (frame.index > frame_infos.size()) {
				// set empty frames if an index is missing
				for (size_t i = frame_infos.size() - 1; i < frame.index; ++i) {
					frame_infos.push_back(nullptr);
				}
			}
			frame_infos.push_back(std::make_shared<TerrainFrameInfo>(
				texture_id_map[frame.texture_id],
				frame.subtex_id));
		}
		if (frame_infos.size() < largest_frame_idx) {
			// insert empty frames at the end if an indices are missing
			for (size_t i = frame_infos.size() - 1; i < largest_frame_idx; ++i) {
				frame_infos.push_back(nullptr);
			}
		}
		layer_infos.emplace_back(frame_infos,
		                         layer.mode,
		                         layer.position,
		                         layer.time_per_frame,
		                         layer.replay_delay);
	}

	// Parse textures
	std::vector<std::shared_ptr<Texture2dInfo>> texture_infos;
	for (auto texture : textures) {
		util::Path texturepath = (file.get_parent() / texture.path);

		if (cache) {
			// Check if already loaded
			auto cached = cache->get_texture(texturepath);
			if (cached) {
				texture_infos.push_back(cached);
				continue;
			}
		}

		auto info = std::make_shared<Texture2dInfo>(parse_texture_file(texturepath));
		texture_infos.push_back(info);
		cache->add_texture(texturepath, info);
	}

	std::shared_ptr<BlendTableInfo> blendtable_info;
	if (blendtable) {
		util::Path tablepath = (file.get_parent() / blendtable.value().path);

		if (cache) {
			// Check if already loaded
			auto cached = cache->get_bltable(tablepath);
			if (cached) {
				blendtable_info = cached;
			}
			else {
				blendtable_info = std::make_shared<BlendTableInfo>(parse_blendtable_file(tablepath));
				cache->add_bltable(tablepath, blendtable_info);
			}
		}
		else {
			blendtable_info = std::make_shared<BlendTableInfo>(parse_blendtable_file(tablepath));
			cache->add_bltable(tablepath, blendtable_info);
		}
	}

	return TerrainInfo(scalefactor, texture_infos, layer_infos, blendtable_info);
}

} // namespace openage::renderer::resources::parser
