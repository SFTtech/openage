// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "parse_blendmask.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "log/message.h"

#include "error/error.h"
#include "renderer/resources/assets/cache.h"
#include "renderer/resources/parser/common.h"
#include "renderer/resources/parser/parse_texture.h"
#include "renderer/resources/texture_info.h"
#include "util/file.h"
#include "util/path.h"
#include "util/strings.h"


namespace openage::renderer::resources::parser {

/**
 * Parse the mask attribute.
 *
 * @param args Arguments from the line with a \p mask attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
blending_mask parse_mask(const std::vector<std::string> &args) {
	blending_mask mask;

	auto dir = 0;
	if (args[0].starts_with("0b")) {
		// discard prefix because std::stoul doesn't understand binary prefixes
		std::string strip = args[0].substr(2, args[2].size() - 1);
		dir = std::stoul(strip, nullptr, 2);
	}
	else {
		dir = std::stoul(args[0]);
	}

	if (dir > 255) [[unlikely]] {
		throw Error(MSG(err) << "Reading .blmask file failed. Reason: "
		                     << "directions value " << dir << " is too large");
	}

	mask.directions = dir;
	mask.texture_id = std::stoul(args[1]);
	mask.subtex_id = std::stoul(args[2]);

	return mask;
}

BlendPatternInfo parse_blendmask_file(const util::Path &path,
                                      const std::shared_ptr<AssetCache> &cache) {
	if (not path.is_file()) [[unlikely]] {
		throw Error(MSG(err) << "Reading .blmask file '"
		                     << path.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto file = path.open();
	auto lines = file.get_lines();

	float scalefactor = 1.0;
	std::vector<TextureData> textures;
	std::vector<blending_mask> masks;

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("version", [&](const std::vector<std::string> &args) {
			size_t version_no = parse_version(args);

			if (version_no != 2) {
				throw Error(MSG(err) << "Reading .blmask file '"
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
		std::make_pair("mask", [&](const std::vector<std::string> &args) {
			masks.push_back(parse_mask(args));
		})};

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(args[0])) [[unlikely]] {
			throw Error(MSG(err) << "Reading .blmask file '"
			                     << path.get_name()
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}

		keywordfuncs[args[0]](args);
	}

	// Order masks by directions value
	std::sort(masks.begin(),
	          masks.end(),
	          [](blending_mask &m1, blending_mask &m2) {
				  return m1.directions < m2.directions;
			  });

	// Create ID map. Resolves IDs used in the file to array indices
	std::unordered_map<size_t, size_t> texture_id_map;
	for (size_t i = 0; i < textures.size(); ++i) {
		texture_id_map.insert(std::make_pair(textures[i].texture_id, i));
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

	return BlendPatternInfo(scalefactor, texture_infos, masks);
}

} // namespace openage::renderer::resources::parser
