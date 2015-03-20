// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "font.h"

#include <fontconfig/fontconfig.h>

#include "log/log.h"
#include "error/error.h"
#include "util/strings.h"
#include "coord/camhud.h"
#include "coord/camgame.h"

namespace openage {


/**
 * using fontconfig for just getting the font file name by a wanted font name and style.
 */
std::string get_font_filename(const char *family, const char *style) {
	//initialize fontconfig
	if (!FcInit()) {
		throw Error{MSG(err) << "Failed to initialize fontconfig."};
	}

	//FcPattern *font_pattern = FcNameParse((const unsigned char *)"DejaVu Serif:style=Book");
	FcPattern *font_pattern = FcPatternBuild(nullptr, FC_FAMILY, FcTypeString, family, nullptr);
	FcPatternBuild(font_pattern, FC_STYLE, FcTypeString, style, nullptr);

	//debug output: display above pattern as parsable string.
	FcChar8 *query_string = FcNameUnparse(font_pattern);
	log::log(MSG(info) << "Font queried: " << query_string);
	free(query_string);

	//tell fontconfig to find the best match
	FcResult font_match_result;
	FcPattern *font_match = FcFontMatch(nullptr, font_pattern, &font_match_result);

	/*
	//debug output: display matching font pattern as parsable string
	FcChar8 *match_string = FcNameUnparse(font_match);
	log::dbg2("resulting font: %s", match_string);
	free(match_string);
	*/

	//get attibute FC_FILE (= filename) of best-matched font
	FcChar8 *font_filename_tmp;
	if (FcPatternGetString(font_match, FC_FILE, 0, &font_filename_tmp) != FcResultMatch) {
		throw Error(MSG(err) << "Fontconfig could not provide font " << family << " " << style);
	}

	std::string font_filename{reinterpret_cast<char *>(font_filename_tmp)};

	log::log(MSG(info) << "Font file: " << font_filename);

	//deinitialize fontconfig.
	FcPatternDestroy(font_match);
	FcPatternDestroy(font_pattern);
	FcFini();

	return font_filename;
}


Font::Font(const char *family, const char *style, unsigned size)
	:
	font_filename{get_font_filename(family, style)} {

	this->internal_font = new FTGLTextureFont(this->font_filename.c_str());

	if (internal_font->Error()) {
		delete this->internal_font;
		throw Error(MSG(err) << "Failed to create FTGL texture font from " << font_filename);
	}

	if (!internal_font->FaceSize(size)) {
		delete this->internal_font;
		throw Error(MSG(err) << "Failed to set font face size to " << size);
	}
}


Font::~Font() {
	delete this->internal_font;
}


void Font::render_static(coord::pixel_t x, coord::pixel_t y, const char *text, int len) {
	this->internal_font->Render(text, len, FTPoint(x, y));
}


void Font::render(coord::pixel_t x, coord::pixel_t y, const char *format, ...) {
	va_list vl;
	va_start(vl, format);
	std::string buf = util::sformat(format, vl);
	va_end(vl);

	this->render_static(x, y, buf.c_str());
}


void Font::render(coord::camhud pos, const char *format, ...) {
	va_list vl;
	va_start(vl, format);
	std::string buf = util::sformat(format, vl);
	va_end(vl);

	this->render_static(pos.x, pos.y, buf.c_str());
}


void Font::render(coord::camgame pos, const char *format, ...) {
	va_list vl;
	va_start(vl, format);
	std::string buf = util::sformat(format, vl);
	va_end(vl);

	this->render_static(pos.x, pos.y, buf.c_str());
}

} //namespace openage
