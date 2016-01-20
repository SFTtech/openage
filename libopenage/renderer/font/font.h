// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include "../../util/hash.h"
#include "font_manager.h"

// Forward Declarations of HarfBuzz stuff!
struct hb_font_t;

namespace openage {
namespace renderer {

constexpr int FREETYPE_UNIT = 64;

using codepoint_t = unsigned int;

/**
 * Holds info about a single glyph.
 */
class Glyph {
public:
	codepoint_t codepoint; //!< Glyph's codepoint.
	int x_offset;          //!< Horizontal distance from origin (current pen position) to glyph's leftmost boundary.
	int y_offset;          //!< Vertical distance from the baseline to glyph's topmost boundary.
	unsigned int width;    //!< Width of the glyph.
	unsigned int height;   //!< Height of the glyph.
	float x_advance;       //!< Advance width of the glyph.
	float y_advance;       //!< Advance height of the glyph.
};

/**
 * Enumeration of the possible font directions.
 */
enum class font_direction {
	left_to_right,
	right_to_left,
	top_to_bottom,
	bottom_to_top
};

/**
 * Description for a font.
 *
 * An instance of font_description is capable of uniquely distinguishing a font.
 */
struct font_description {
	std::string font_file;    //!< Path to the font's file.
	unsigned int size;        //!< Points size of the font.
	font_direction direction; //!< The direction of the font.
	std::string language;     //!< Language of the font.
	std::string script;       //!< The font's script.

	/**
	 * Constructs a font_description instance.
	 *
	 * @param font_file: The path to fon't font.
	 * @param size: The size of the font in points.
	 * @param direction: The direction of font.
	 * @param language: The font's language.
	 * @param script: The font's script.
	 */
	font_description(const std::string &font_file,
	                 unsigned int size,
	                 font_direction direction = font_direction::left_to_right,
	                 const std::string &language = "en",
	                 const std::string &script = "Latn");

	/**
	 * Constructs a font_description instance.
	 *
	 * This constructor uses fontconfig to determine a font file for the specified font family and style.
	 *
	 * @param family: The font family.
	 * @param style: The font style.
	 * @param size: The size of the font in points.
	 */
	font_description(const char *family, const char *style, unsigned int size);

	font_description(const font_description &other);

	font_description &operator=(const font_description &other);

	bool operator==(const font_description &other) const;

	bool operator!=(const font_description &other) const;

};

class Font {

public:
	/**
	 * Create a font instance from the description.
	 *
	 * @param description: the font description.
	 */
	Font(const font_description &description);

	/**
	 * This constructor is used by the font manager to create a
	 * new font instance from the description.
	 *
	 * @param font_manager: The font manager.
	 * @param description: the font description.
	 */
	Font(FontManager *font_manager, const font_description &description);

	virtual ~Font();

	/**
	 * Get the typographic ascender of font.
	 *
	 * @returns The ascender of font.
	 */
	float get_ascender() const;

	/**
	 * Get the typographic descender of font.
	 *
	 * @returns The descender of font.
	 */
	float get_descender() const;

	/**
	 * Get the line spacing of font.
	 *
	 * @returns The font's line height.
	 */
	float get_line_height() const;

	/**
	 * Get the kerning adjustment between two glyphs.
	 *
	 * @param left_glyph: The first glyph.
	 * @param right_glyph: The next glyph.
	 * @returns The kerning adjustment.
	 */
	float get_horizontal_kerning(codepoint_t left_glyph, codepoint_t right_glyph) const;

	/**
	 * Get the advance width of a particular string.
	 *
	 * @param text: The string for which the width is to be determined.
	 * @returns The advance width of the specified string.
	 */
	float get_advance_width(const std::string &text) const;

	/**
	 * Get the list of glyphs for a particular string.
	 *
	 * @param text: the string for which the glyphs are to be retrieved.
	 * @returns The list of the glyphs.
	 */
	std::vector<codepoint_t> get_glyphs(const std::string &text) const;

	/**
	 * Load a particular glyph's info and retrieves the glyph's bitmap data.
	 *
	 * @param codepoint: The glyph.
	 * @param glyph: The glyph's info is loaded in to this object.
	 * @returns The glyph's bitmap data.
	 */
	std::unique_ptr<unsigned char> load_glyph(codepoint_t codepoint, Glyph &glyph) const;

private:
	/**
	 * Initializes the font's face and creates a harfbuzz font instance.
	 *
	 * @param ft_library: The freetype library that should be used to load the font's face.
	 */
	void initialize(FT_Library ft_library);

public:
	/**
	 * The description of the font.
	 * @see font_description
	 */
	font_description description;

private:
	// Font's created without the use of FontManager are standalone and have their own FreeTypeLibrary instance
	std::unique_ptr<FreeTypeLibrary> freetype_library;

	// The HarfBuzz font instance that drives the operations of this font
	hb_font_t *hb_font;

};

}} // openage::renderer

namespace std {

template<>
struct hash<openage::renderer::font_description> {
	size_t operator()(const openage::renderer::font_description &fd) const {
		size_t hash = std::hash<std::type_index>()(std::type_index(typeid(openage::renderer::font_description)));
		hash = openage::util::hash_combine(hash, std::hash<std::string>()(fd.font_file));
		hash = openage::util::hash_combine(hash, std::hash<unsigned int>()(fd.size));
		return hash;
	}
};

}
