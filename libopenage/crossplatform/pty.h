// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#ifdef __APPLE__
#  include <util.h>
#elif _WIN32
// TODO not yet implemented
#else
#  include <pty.h>
#endif
