// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "parse_blendtable.h"

#include <functional>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "renderer/resources/assets/cache.h"
#include "renderer/resources/parser/common.h"
#include "renderer/resources/parser/parse_blendmask.h"
#include "renderer/resources/terrain/blendpattern_info.h"
#include "renderer/resources/terrain/blendtable_info.h"
#include "util/file.h"
#include "util/path.h"
#include "util/strings.h"


namespace openage::renderer::resources::parser {

/**
 * Parse the blendtable attribute.
 *
 * @param args Lines composing the matrix from a \p blendtable attribute.
 *             The first line is expected to be the line containing attribute keyword.
 *             The last line is expected to be the line containing the closing bracket.
 *
 * @return Struct containing the attribute data.
 */
std::vector<size_t> parse_table(const std::vector<std::string> &lines) {
	std::vector<size_t> entries{};

	for (size_t i = 1; i < lines.size() - 1; ++i) {
		std::vector<std::string> line_args{util::split(lines[i], ' ')};
		for (auto arg : line_args) {
			entries.push_back(std::stoul(arg));
		}
	}

	return entries;
}

/**
 * Parse the pattern attribute.
 *
 * @param args Arguments from the line with a \p pattern attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
PatternData parse_pattern(const std::vector<std::string> &args) {
	// TODO: Splitting at the space char assumes that the path string contains no
	// space. While the space char is not allowed because of nyan naming requirements,
	// it should result in an error if wrongly used here.
	PatternData pattern;

	pattern.pattern_id = std::stoul(args[1]);

	// Call substr() to get rid of the quotes
	pattern.path = args[2].substr(1, args[2].size() - 2);

	return pattern;
}

BlendTableInfo parse_blendtable_file(const util::Path &path,
                                     const std::shared_ptr<AssetCache> &cache) {
	if (not path.is_file()) [[unlikely]] {
		throw Error(MSG(err) << "Reading .bltable file '"
		                     << path.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto file = path.open();
	auto lines = file.get_lines();

	std::vector<size_t> blendtable;
	std::vector<PatternData> patterns;

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("version", [&](const std::vector<std::string> &args) {
			size_t version_no = parse_version(args);

			if (version_no != 1) {
				throw Error(MSG(err) << "Reading .bltable file '"
			                         << path.get_name()
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("blendtable", [&](const std::vector<std::string> &args) {
			blendtable = parse_table(args);
		}),
		std::make_pair("pattern", [&](const std::vector<std::string> &args) {
			patterns.push_back(parse_pattern(args));
		})};

	for (size_t i = 0; i < lines.size(); ++i) {
		// Skip empty lines and comments
		auto line = lines[i];
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(args[0])) [[unlikely]] {
			throw Error(MSG(err) << "Reading .bltable file '"
			                     << path.get_name()
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}

		if (args[0] == "blendtable") {
			// read all lines of the matrix
			// TODO: better parsing for matrix values
			std::vector<std::string> mat_lines{};
			mat_lines.push_back(line);

			while (not line.starts_with("]")) {
				i += 1;
				if (i >= lines.size()) {
					throw Error(MSG(err) << "Reading .bltable file '"
					                     << path.get_name()
					                     << "' failed. Reason: Matrix for keyword "
					                     << args[0] << " is malformed");
				}
				line = lines[i];
				mat_lines.push_back(line);
			}

			keywordfuncs[args[0]](mat_lines);
		}
		else {
			keywordfuncs[args[0]](args);
		}
	}

	std::vector<std::shared_ptr<BlendPatternInfo>> pattern_infos;
	for (auto pattern : patterns) {
		util::Path maskpath = (path.get_parent() / pattern.path);

		if (cache && cache->check_blpattern_cache(maskpath)) {
			// already loaded
			pattern_infos.push_back(cache->get_blpattern(maskpath));
		}
		else {
			// load (and cache if possible)
			auto info = std::make_shared<BlendPatternInfo>(parse_blendmask_file(maskpath));
			pattern_infos.push_back(info);
			if (cache) {
				cache->add_blpattern(maskpath, info);
			}
		}
	}

	return BlendTableInfo(blendtable, pattern_infos);
}

} // namespace openage::renderer::resources::parser
