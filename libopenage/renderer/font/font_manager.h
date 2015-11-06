// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_FONT_FONT_MANAGER_H_
#define OPENAGE_RENDERER_FONT_FONT_MANAGER_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include <memory>
#include <string>
#include <unordered_map>

#include "../../error/error.h"

namespace openage {
namespace renderer {

class FreeTypeLibrary {

public:
	FT_Library ft_library;

public:
	FreeTypeLibrary() {
		if (FT_Init_FreeType(&this->ft_library) != 0) {
			throw Error{MSG(err) << "Failed to initialize freetype library."};
		}
	}

	~FreeTypeLibrary() {
		FT_Done_FreeType(this->ft_library);
	}

	FreeTypeLibrary(const FreeTypeLibrary &copy) = delete;

	FreeTypeLibrary &operator=(const FreeTypeLibrary &copy) = delete;

	FreeTypeLibrary(FreeTypeLibrary &&other) = delete;

	FreeTypeLibrary &operator=(FreeTypeLibrary &&other) = delete;

};

class Font;

class FontManager {

public:
	/**
	 * Gets the filepath of a particular font family and style.
	 *
	 * @param family: The font family.
	 * @param style: The font style.
	 * @returns The path to font's file.
	 */
	static std::string get_font_filename(const char *family, const char *style);

public:
	FontManager();

	~FontManager();

	/**
	 * Get the freetype library instance.
	 */
	FT_Library get_ft_library();

	/**
	 * Retrieves a font.
	 *
	 * @param family: The font family.
	 * @param style: The font style.
	 * @param size: The size of the font in points.
	 * @returns The pointer to font instance.
	 */
	Font *get_font(const char *family, const char *style, unsigned int size);

	/**
	 * Retrieves a font.
	 *
	 * @param font_file: The path to font's file.
	 * @param size: The size of the font in points.
	 * @returns The pointer to font instance.
	 */
	Font *get_font(const char* font_file, unsigned int size);

private:
	// The freetype library instance
	FreeTypeLibrary library;

	// Font cache. the hash of font's description is used as the key
	std::unordered_map<size_t, std::unique_ptr<Font>> fonts;

};

}} // openage::renderer


#endif
