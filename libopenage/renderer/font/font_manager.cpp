// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "font_manager.h"

#include "font.h"

#include <fontconfig/fontconfig.h>

namespace openage {
namespace renderer {

std::string FontManager::get_font_filename(const char *family, const char *style) {
	// Initialize fontconfig
	if (!FcInit()) {
		throw Error{MSG(err) << "Failed to initialize fontconfig."};
	}

	FcPattern *font_pattern = FcPatternBuild(nullptr, FC_FAMILY, FcTypeString, family, nullptr);
	FcPatternBuild(font_pattern, FC_STYLE, FcTypeString, style, nullptr);

	FcChar8 *query_string = FcNameUnparse(font_pattern);
	log::log(MSG(info) << "Font queried: " << query_string);
	free(query_string);

	// tell fontconfig to find the best match
	FcResult font_match_result;
	FcPattern *font_match = FcFontMatch(nullptr, font_pattern, &font_match_result);

	// get attibute FC_FILE (= filename) of best-matched font
	FcChar8 *font_filename_tmp;
	if (FcPatternGetString(font_match, FC_FILE, 0, &font_filename_tmp) != FcResultMatch) {
		throw Error(MSG(err) << "Fontconfig could not provide font " << family << " " << style);
	}

	std::string font_filename{reinterpret_cast<char *>(font_filename_tmp)};

	log::log(MSG(info) << "Font file: " << font_filename);

	// deinitialize fontconfig.
	FcPatternDestroy(font_match);
	FcPatternDestroy(font_pattern);
	FcFini();

	return font_filename;
}

FontManager::FontManager() {
	// Empty
}

FontManager::~FontManager() {
	// Empty
}

FT_Library FontManager::get_ft_library() {
	return this->library.ft_library;
}

Font *FontManager::get_font(const char *family, const char *style, unsigned int size) {
	std::string font_filename = FontManager::get_font_filename(family, style);
	return this->get_font(font_filename.c_str(), size);
}

Font *FontManager::get_font(const char *font_file, unsigned int size) {
	// Check if the font was already created
	font_description fd{font_file, size};
	size_t key = std::hash<font_description>()(fd);
	auto it = this->fonts.find(key);
	if (it != this->fonts.end()) {
		return it->second.get();
	}

	// Create a new font
	std::unique_ptr<Font> font = std::make_unique<Font>(this, fd);
	auto inserted = this->fonts.emplace(key, std::move(font));
	return inserted.first->second.get();
}

}} // openage::renderer
