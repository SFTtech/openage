#include "console.h"

#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>

#include "../engine.h"
#include "../font.h"
#include "../log.h"
#include "../util/color.h"
#include "../util/error.h"
#include "../util/strings.h"

namespace engine {
namespace console {

bool visible = false;

coord::camhud topleft {1, 0};
coord::camhud bottomright {0, 1};

util::col bgcol;
util::col fgcol;
std::vector<const char *> messages;
Font *font = nullptr;

int spacing = 7;

void write(const char *text) {
	char *store_text = util::copy(text);
	messages.push_back(store_text);
}

} //namespace console
} //namespace engine
