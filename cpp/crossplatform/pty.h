#ifndef OPENAGE_CROSSPLATFORM_PTY_H_
#define OPENAGE_CROSSPLATFORM_PTY_H_

#ifdef __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#endif

#endif
