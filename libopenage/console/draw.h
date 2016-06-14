// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {
namespace util {
class FD;
} // openage::util

namespace console {

class Buf;
class Console;

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
