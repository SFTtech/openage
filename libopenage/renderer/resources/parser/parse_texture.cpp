// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#include "parse_texture.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "renderer/resources/parser/common.h"
#include "renderer/resources/texture_subinfo.h"
#include "util/file.h"
#include "util/path.h"
#include "util/strings.h"


namespace openage::renderer::resources::parser {

/// Tries to guess the alignment of image rows based on image parameters. Kinda
/// black magic and might not actually work.
/// @param width in pixels of the image
static constexpr size_t guess_row_alignment(size_t width) {
	// Use the highest possible alignment for even-width images.
	if (width % 8 == 0) {
		return 8;
	}
	if (width % 4 == 0) {
		return 4;
	}
	if (width % 2 == 0) {
		return 2;
	}

	// Bail with a sane value.
	return 4;
}

/**
 * Parse the imagefile attribute.
 *
 * @param args Arguments from the line with a \p imagefile attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Path to the image resource.
 */
std::string parse_imagefile(const std::vector<std::string> &args) {
	// TODO: Splitting at the space char assumes that the path string contains no
	// space. While the space char is not allowed because of nyan naming requirements,
	// it should result in an error if wrongly used here.

	// Call substr() to get rid of the quotes
	return args[1].substr(1, args[1].size() - 2);
}

/**
 * Parse the size attribute.
 *
 * @param args Arguments from the line with a \p size attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
SizeData parse_size(const std::vector<std::string> &args) {
	SizeData size;

	size.width = std::stoul(args[1]);
	size.height = std::stoul(args[2]);

	return size;
}

/**
 * Parse the pxformat attribute.
 *
 * @param args Arguments from the line with a \p pxformat attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
PixelFormatData parse_pxformat(const std::vector<std::string> &args) {
	PixelFormatData pxformat;

	// Only accepted format
	pxformat.format = pixel_format::rgba8;

	// Optional arguments
	auto keywordfuncs = std::unordered_map<std::string, std::function<void(std::vector<std::string>)>>{
		std::make_pair("cbits", [&](std::vector<std::string> keywordargs) {
			if (keywordargs[1] == "True") {
				pxformat.cbits = true;
			}
			else if (keywordargs[1] == "False") {
				pxformat.cbits = false;
			}
		})};

	// Optional arguments
	for (size_t i = 2; i < args.size(); ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(keywordargs[0])) [[unlikely]] {
			throw Error(MSG(err) << "Keyword argument "
			                     << keywordargs[0]
			                     << " of 'pxformat' attribute is not defined");
		}

		keywordfuncs[keywordargs[0]](keywordargs);
	}

	return pxformat;
}

/**
 * Parse the subtex attribute.
 *
 * @param args Arguments from the line with a \p subtex attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
SubtextureData parse_subtex(const std::vector<std::string> &args) {
	SubtextureData subtex;

	subtex.xpos = std::stoi(args[1]);
	subtex.ypos = std::stoi(args[2]);
	subtex.xsize = std::stoul(args[3]);
	subtex.ysize = std::stoul(args[4]);
	subtex.xanchor = std::stoi(args[5]);
	subtex.yanchor = std::stoi(args[6]);

	return subtex;
}

Texture2dInfo parse_texture_file(const util::Path &path) {
	if (not path.is_file()) [[unlikely]] {
		throw Error(MSG(err) << "Reading .texture file '"
		                     << path.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto file = path.open();
	auto lines = file.get_lines();

	std::string imagefile;
	SizeData size;
	PixelFormatData pxformat;
	std::vector<SubtextureData> subtexs;

	auto keywordfuncs = std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>>{
		std::make_pair("version", [&](const std::vector<std::string> &args) {
			size_t version_no = parse_version(args);

			if (version_no != 1) {
				throw Error(MSG(err) << "Reading .texture file '"
			                         << path.get_name()
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("imagefile", [&](const std::vector<std::string> &args) {
			imagefile = parse_imagefile(args);
		}),
		std::make_pair("size", [&](const std::vector<std::string> &args) {
			size = parse_size(args);
		}),
		std::make_pair("pxformat", [&](const std::vector<std::string> &args) {
			pxformat = parse_pxformat(args);
		}),
		std::make_pair("subtex", [&](const std::vector<std::string> &args) {
			subtexs.push_back(parse_subtex(args));
		})};

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		// TODO: Avoid double lookup with keywordfuncs.find(args[0])
		if (not keywordfuncs.contains(args[0])) [[unlikely]] {
			throw Error(MSG(err) << "Reading .texture file '"
			                     << path.get_name()
			                     << "' failed. Reason: Keyword "
			                     << args[0] << " is not defined");
		}
		keywordfuncs[args[0]](args);
	}

	std::vector<Texture2dSubInfo> subinfos;

	for (auto subtex : subtexs) {
		subinfos.emplace_back(subtex.xpos,
		                      subtex.ypos,
		                      subtex.xsize,
		                      subtex.ysize,
		                      subtex.xanchor,
		                      subtex.yanchor,
		                      size.width,
		                      size.height);
	}

	auto imagepath = path.get_parent() / imagefile;

	auto align = guess_row_alignment(size.width);
	return Texture2dInfo(size.width, size.height, pxformat.format, imagepath, align, std::move(subinfos));
}

} // namespace openage::renderer::resources::parser
