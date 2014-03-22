#ifndef _ENGINE_CONSOLE_DRAW_H_
#define _ENGINE_CONSOLE_DRAW_H_

#include "buf.h"

#include "../util/fds.h"

namespace engine {
namespace console {
namespace draw {

void to_terminal(Buf *buf, util::FD *fd, bool clear = false);

} //namespace draw
} //namespace console
} //namespace engine

#endif //_ENGINE_CONSOLE_DRAW_H_
