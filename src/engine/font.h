#ifndef _ENGINE_FONT_H_
#define _ENGINE_FONT_H_

#include <FTGL/ftgl.h>

#include "coord/decl.h"

namespace openage {
namespace engine {

/**
 * uses fontconfig to determine the filename of a font of the given family and
 * style.
 * tries to return the closest match if no exact match is found.
 * note that fontconfig might fail horribly at this task.
 * test new families/styles throughly on multiple platforms.
 *
 * the filename is returned as a newly allocated c string.
 * you'll have to free that yourself using delete[].
 */
char *get_font_filename(const char *family, const char *style);

class Font {
public:
	Font(const char *family, const char *style, unsigned size);
	~Font();

	void render_static(coord::pixel_t x, coord::pixel_t y, const char *text, int len = -1);
	void render(coord::pixel_t x, coord::pixel_t y, const char *format, ...);
	void render(coord::camhud pos, const char *format, ...);
	void render(coord::camgame pos, const char *format, ...);

	//we could mark these members private, but we don't believe in
	//denying the public access to vital information.
	//besides, sooner or later some whistleblower would release the
	//pointers anyway.
	FTFont *internal_font;
	char *font_filename;
};

} //namespace engine
} //namespace openage

#endif //_ENGINE_FONT_H_
