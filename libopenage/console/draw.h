// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CONSOLE_DRAW_H_
#define OPENAGE_CONSOLE_DRAW_H_

#include "buf.h"

#include "console.h"
#include "../coord/camhud.h"
#include "../util/fds.h"

namespace openage {
namespace console {

namespace draw {

/**
 * experimental and totally inefficient opengl draw of a terminal buffer.
 */
void to_opengl(Console *console);

/**
 * very early and inefficient printing of the console to a pty.
 */
void to_terminal(Buf *buf, util::FD *fd,
                 bool clear=false);

}}} // openage::console::draw

#endif
