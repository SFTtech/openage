// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "parse_palette.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "renderer/resources/parser/common.h"
#include "util/file.h"
#include "util/path.h"
#include "util/strings.h"


namespace openage::renderer::resources::parser {

/**
 * Parse the entries attribute.
 *
 * @param args Arguments from the line with a \p entries attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Version number.
 */
size_t parse_entries(const std::vector<std::string> &args) {
	return std::stoul(args[1]);
}

/**
 * Parse the colours attribute.
 *
 * @param args Lines composing the matrix from a \p colours attribute.
 *             The first line is expected to be the line containing attribute keyword.
 *             The last line is expected to be the line containing the closing bracket.
 *
 * @return Struct containing the attribute data.
 */
std::vector<uint8_t> parse_colours(const std::vector<std::string> &lines) {
	std::vector<uint8_t> entries{};

	for (size_t i = 1; i < lines.size() - 1; ++i) {
		std::vector<std::string> line_args{util::split(lines[i], ' ')};
		for (auto arg : line_args) {
			auto channel = std::stoul(arg);

			if (channel > 255) [[unlikely]] {
				throw Error(MSG(err) << "Reading .opal file failed. Reason: "
				                     << "color channel value " << channel << " is too large");
			}

			entries.push_back(channel);
		}
	}

	return entries;
}

PaletteInfo parse_palette_file(const util::Path &path) {
	if (not path.is_file()) [[unlikely]] {
		throw Error(MSG(err) << "Reading .opal file '"
		                     << path.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto file = path.open();
	auto lines = file.get_lines();

	size_t entries = 0;
	std::vector<uint8_t> colours;

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("version", [&](const std::vector<std::string> &args) {
			size_t version_no = parse_version(args);

			if (version_no != 1) {
				throw Error(MSG(err) << "Reading .opal file '"
			                         << path.get_name()
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("entries", [&](const std::vector<std::string> &args) {
			entries = parse_entries(args);
		}),
		std::make_pair("colours", [&](const std::vector<std::string> &args) {
			colours = parse_colours(args);
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
			throw Error(MSG(err) << "Reading .opal file '"
			                     << path.get_name()
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}

		if (args[0] == "colours") {
			// read all lines of the matrix
			// TODO: better parsing for matrix values
			std::vector<std::string> mat_lines{};
			mat_lines.push_back(line);

			while (not line.starts_with("]")) {
				i += 1;
				if (i >= lines.size()) {
					throw Error(MSG(err) << "Reading .opal file '"
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

	return PaletteInfo(colours);
}

} // namespace openage::renderer::resources::parser
