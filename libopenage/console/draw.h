// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

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
