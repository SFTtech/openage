// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include <utility>

#include "../../error/error.h"
#include "../../util/compiler.h"

namespace openage::renderer {

font_description::font_description(std::string font_file,
                                   unsigned int size,
                                   font_direction direction,
                                   std::string language,
                                   std::string script)
	:
	font_file{std::move(font_file)},
	size{size},
	direction{direction},
	language{std::move(language)},
	script{std::move(script)} {
	// Empty
}

font_description::font_description(const char *family, const char *style, unsigned int size)
	:
	font_description{FontManager::get_font_filename(family, style), size} {
	// Empty
}

font_description &font_description::operator=(const font_description &other) {
	if (this != &other) {
		this->font_file = other.font_file;
		this->size = other.size;
		this->direction = other.direction;
		this->language = other.language;
		this->script = other.script;
	}

	return *this;
}

font_description::font_description(const font_description &other)
	:
	font_file{other.font_file},
	size{other.size},
	direction{other.direction},
	language{other.language},
	script{other.script} {
	// Empty
}

bool font_description::operator==(const font_description &other) const {
	return this->font_file == other.font_file &&
		this->size == other.size &&
		this->direction == other.direction &&
		this->language == other.language &&
		this->script == other.script;
}

bool font_description::operator!=(const font_description &other) const {
	return !(*this == other);
}

static hb_direction_t get_hb_font_direction(const font_description &description) {
	switch(description.direction) {
	case font_direction::left_to_right:
		return HB_DIRECTION_LTR;
	case font_direction::right_to_left:
		return HB_DIRECTION_RTL;
	case font_direction::top_to_bottom:
		return HB_DIRECTION_TTB;
	case font_direction::bottom_to_top:
		return HB_DIRECTION_BTT;
	default:
		return HB_DIRECTION_INVALID;
	}
}

static hb_language_t get_hb_font_language(const font_description &description) {
	return hb_language_from_string(description.language.c_str(), -1);
}

static hb_script_t get_hb_font_script(const font_description &description) {
	return hb_script_from_string(description.script.c_str(), -1);
}

Font::Font(const font_description &description)
	:
	description{description},
	freetype_library{nullptr},
	hb_font{nullptr} {

	this->freetype_library = std::make_unique<FreeTypeLibrary>();
	this->initialize(this->freetype_library->ft_library);
}

Font::Font(FontManager *font_manager, const font_description &description)
	:
	description{description},
	freetype_library{nullptr},
	hb_font{nullptr} {

	this->initialize(font_manager->get_ft_library());
}

void Font::initialize(FT_Library ft_library) {
	FT_Face ft_face;
	if (unlikely(FT_New_Face(ft_library, this->description.font_file.c_str(), 0, &ft_face))) {
		throw Error(MSG(err) << "Failed to create font from " << this->description.font_file);
	}
	if (unlikely(FT_Set_Char_Size(ft_face, 0, this->description.size * FREETYPE_UNIT, 72, 72))) {
		throw Error(MSG(err) << "Failed to set font face size to " << this->description.size);
	}

	hb_ft_font_create_referenced(ft_face);
	// lambda with static_cast to help gcc understand this is ok
	this->hb_font = hb_ft_font_create(ft_face, [] (void *user_data) -> void {
		FT_Done_Face(static_cast<FT_FaceRec_ *>(user_data));
	});
}

Font::~Font() {
	// Destroy HarfBuzz font
	// HarfBuzz will take care of destroying the FT_Face instance
	if (this->hb_font) {
		hb_font_destroy(this->hb_font);
		this->hb_font = nullptr;
	}
}

float Font::get_ascender() const {
	FT_Face ft_face = hb_ft_font_get_face(this->hb_font);
	return static_cast<float>(ft_face->size->metrics.ascender)/FREETYPE_UNIT;
}

float Font::get_descender() const {
	FT_Face ft_face = hb_ft_font_get_face(this->hb_font);
	return static_cast<float>(ft_face->size->metrics.descender)/FREETYPE_UNIT;
}

float Font::get_line_height() const {
	FT_Face ft_face = hb_ft_font_get_face(this->hb_font);

	if (FT_IS_SCALABLE(ft_face)) {
		float units_to_pixels_conversion = static_cast<float>(ft_face->size->metrics.y_ppem)/ft_face->units_per_EM;
		return (ft_face->bbox.yMax - ft_face->bbox.yMin) * units_to_pixels_conversion;
	}

	return static_cast<float>(ft_face->size->metrics.height)/FREETYPE_UNIT;
}

float Font::get_horizontal_kerning(codepoint_t left_glyph, codepoint_t right_glyph) const {
	int kerning = hb_font_get_glyph_h_kerning(this->hb_font, left_glyph, right_glyph);
	return static_cast<float>(kerning)/FREETYPE_UNIT;
}

float Font::get_advance_width(const std::string &text) const {
	hb_buffer_t *buffer = hb_buffer_create();
	hb_buffer_set_direction(buffer, get_hb_font_direction(this->description));
	hb_buffer_set_script(buffer, get_hb_font_script(this->description));
	hb_buffer_set_language(buffer, get_hb_font_language(this->description));
	hb_buffer_add_utf8(buffer, text.c_str(), text.length(), 0, text.length());
	hb_shape(this->hb_font, buffer, nullptr, 0);

	float advance_width = 0.0f;

	unsigned int glyph_count = 0;
	hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buffer, nullptr);
	for (unsigned int i = 0; i < glyph_count; i++) {
		advance_width += static_cast<float>(glyph_pos[i].x_advance)/FREETYPE_UNIT;

		if (i > 0) {
			codepoint_t glyph = glyph_info[i].codepoint;
			codepoint_t previous_glyph = glyph_info[i - 1].codepoint;
			advance_width += this->get_horizontal_kerning(previous_glyph, glyph);
		}
	}

	hb_buffer_destroy(buffer);

	return advance_width;
}

std::vector<codepoint_t> Font::get_glyphs(const std::string &text) const {
	hb_buffer_t *buffer = hb_buffer_create();
	hb_buffer_set_direction(buffer, get_hb_font_direction(this->description));
	hb_buffer_set_script(buffer, get_hb_font_script(this->description));
	hb_buffer_set_language(buffer, get_hb_font_language(this->description));
	hb_buffer_add_utf8(buffer, text.c_str(), text.length(), 0, text.length());
	hb_shape(this->hb_font, buffer, nullptr, 0);

	unsigned int glyph_count = 0;
	hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);

	std::vector<codepoint_t> glyphs;
	glyphs.resize(glyph_count);
	for (unsigned int i = 0; i < glyph_count; i++) {
		glyphs[i] = glyph_info[i].codepoint;
	}

	hb_buffer_destroy(buffer);

	return glyphs;
}

std::unique_ptr<unsigned char[]> Font::load_glyph(codepoint_t codepoint, Glyph &glyph) const {
	FT_Face ft_face = hb_ft_font_get_face(this->hb_font);
	if (unlikely(FT_Load_Glyph(ft_face, codepoint, FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING | FT_LOAD_RENDER))) {
		return nullptr;
	}

	glyph.codepoint = codepoint;
	glyph.x_offset = ft_face->glyph->bitmap_left;
	glyph.y_offset = ft_face->glyph->bitmap_top;
	glyph.width = ft_face->glyph->bitmap.width;
	glyph.height = ft_face->glyph->bitmap.rows;
	glyph.x_advance = static_cast<float>(ft_face->glyph->advance.x)/FREETYPE_UNIT;
	glyph.y_advance = static_cast<float>(ft_face->glyph->advance.y)/FREETYPE_UNIT;

	auto glyph_data = std::make_unique<unsigned char[]>(glyph.width * glyph.height);
	for (size_t i = 0; i < glyph.height; i++) {
		memcpy(
			&glyph_data[i * glyph.width],
			ft_face->glyph->bitmap.buffer + (glyph.height - i - 1) * glyph.width,
			glyph.width * sizeof(unsigned char)
		);
	}
	return glyph_data;
}

} // openage::renderer
