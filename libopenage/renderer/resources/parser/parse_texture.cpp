// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "parse_texture.h"
#include "../../../datastructure/constexpr_map.h"
#include "../../../error/error.h"
#include "../../../util/strings.h"

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
 * Parse the file version attribute.
 *
 * @param args Arguments from the line with a \p version attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Version number.
 */
size_t parse_texversion(std::vector<std::string> args) {
	return std::stoul(args[1]);
}

/**
 * Parse the imagefile attribute.
 *
 * @param args Arguments from the line with a \p imagefile attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Path to the image resource.
 */
std::string parse_imagefile(std::vector<std::string> args) {
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
SizeData parse_size(std::vector<std::string> args) {
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
PixelFormatData parse_pxformat(std::vector<std::string> args) {
	PixelFormatData pxformat;

	// Only accepted format
	pxformat.format = pixel_format::rgba8;

	// Optional arguments
	const auto keywordfuncs = datastructure::create_const_map<std::string, std::function<void(std::vector<std::string>)>>(
		std::make_pair("cbits", [&](std::vector<std::string> keywordargs) {
			if (keywordargs[1] == "True") {
				pxformat.cbits = true;
			}
			else if (keywordargs[1] == "False") {
				pxformat.cbits = false;
			}
		}));

	// Optional arguments
	for (size_t i = 2; i < args.size(); ++i) {
		std::vector<std::string> keywordargs{util::split(args[i], '=')};

		if (unlikely(!keywordfuncs.contains(keywordargs[0]))) {
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
SubtextureData parse_subtex(std::vector<std::string> args) {
	SubtextureData subtex;

	subtex.xpos = std::stoul(args[1]);
	subtex.ypos = std::stoul(args[2]);
	subtex.xsize = std::stoul(args[3]);
	subtex.ysize = std::stoul(args[4]);
	subtex.xanchor = std::stoi(args[5]);
	subtex.yanchor = std::stoi(args[6]);

	return subtex;
}

Texture2dInfo parse_texture_file(const util::Path &file) {
	if (unlikely(!file.is_file())) {
		throw Error(MSG(err) << "Reading .texture file '"
		                     << file.get_name()
		                     << "' failed. Reason: File not found");
	}

	auto content = file.open();
	auto lines = content.get_lines();

	std::string imagefile;
	SizeData size;
	PixelFormatData pxformat;
	std::vector<SubtextureData> subtexs;

	const auto keywordfuncs = datastructure::create_const_map<std::string, std::function<void(std::vector<std::string>)>>(
		std::make_pair("version", [&](std::vector<std::string> args) {
			size_t version_no = parse_texversion(args);

			if (version_no != 1) {
				throw Error(MSG(err) << "Reading .texture file '"
			                         << file.get_name()
			                         << "' failed. Reason: Version "
			                         << version_no << " not supported");
			}
		}),
		std::make_pair("imagefile", [&](std::vector<std::string> args) {
			imagefile = parse_imagefile(args);
		}),
		std::make_pair("size", [&](std::vector<std::string> args) {
			size = parse_size(args);
		}),
		std::make_pair("pxformat", [&](std::vector<std::string> args) {
			pxformat = parse_pxformat(args);
		}),
		std::make_pair("subtex", [&](std::vector<std::string> args) {
			subtexs.push_back(parse_subtex(args));
		}));

	for (auto line : lines) {
		// Skip empty lines and comments
		if (line.empty() || line.substr(0, 1) == "#") {
			continue;
		}
		std::vector<std::string> args{util::split(line, ' ')};

		if (unlikely(!keywordfuncs.contains(args[0]))) {
			throw Error(MSG(err) << "Reading .texture file '"
			                     << file.get_name()
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
		                      subtex.yanchor);
	}

	std::shared_ptr<util::Path> image = std::make_shared<util::Path>(file.get_parent() / imagefile);

	auto align = guess_row_alignment(size.width);
	return Texture2dInfo(size.width, size.height, pxformat.format, align, image, std::move(subinfos));
}

} // namespace openage::renderer::resources::parser
