#include "font.h"

#include <fontconfig/fontconfig.h>

#include "../log/log.h"
#include "../util/error.h"
#include "../util/strings.h"

namespace openage {
namespace engine {

/**
using fontconfig for just getting the font file name by a wanted font name and style.
*/
char *get_font_filename(const char *family, const char *style) {
	//initialize fontconfig
	if (!FcInit()) {
		throw util::Error("Failed to initialize fontconfig.");
	}

	//FcPattern *font_pattern = FcNameParse((const unsigned char *)"DejaVu Serif:style=Book");
	FcPattern *font_pattern = FcPatternBuild(nullptr, FC_FAMILY, FcTypeString, family, nullptr);
	FcPatternBuild(font_pattern, FC_STYLE, FcTypeString, style, nullptr);

	//debug output: display above pattern as parsable string.
	FcChar8 *query_string = FcNameUnparse(font_pattern);
	log::dbg2("queried font: %s", query_string);
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
		throw util::Error("fontconfig could not provide font %s %s", family, style);
	}

	//copy the font filename because it will be freed when the pattern is destroyed.
	char *font_filename = util::copy((const char *)font_filename_tmp);

	log::dbg2("returning font file %s", font_filename);

	//deinitialize fontconfig.
	FcPatternDestroy(font_match);
	FcPatternDestroy(font_pattern);
	FcFini();

	return font_filename;
}

} //namespace engine
} //namespace openage
