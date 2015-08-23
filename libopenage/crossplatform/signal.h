// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CROSSPLATFORM_SIGNAL_H_
#define OPENAGE_CROSSPLATFORM_SIGNAL_H_

#include <signal.h>

#ifdef __APPLE__
	typedef void (*sighandler_t)(int);
#endif

#endif
