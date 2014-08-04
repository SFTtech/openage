#ifndef _INIT_H_
#define _INIT_H_

#include "util/dir.h"

namespace openage {

/**
engine initialization method.
opens a window and initializes the OpenGL context.
*/
void init(util::Dir &data_dir, const char *windowtitle);

/**
engine de-initialization method.
call to cleanly shutdown the application.
*/
void destroy();

} //namespace openage

#endif //_INIT_H_
