#ifndef _ENGINE_CONSOLE_DRAW_H_
#define _ENGINE_CONSOLE_DRAW_H_

#include "buf.h"

#include "../coord/camhud.h"
#include "../font.h"
#include "../util/fds.h"

namespace openage {
namespace console {
namespace draw {

void to_opengl(Buf *buf, Font *font, coord::camhud bottomleft, coord::camhud charsize);
void to_terminal(Buf *buf, util::FD *fd, bool clear = false);

} //namespace draw
} //namespace console
} //namespace openage

#endif //_ENGINE_CONSOLE_DRAW_H_
