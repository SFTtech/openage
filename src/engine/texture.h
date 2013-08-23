#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "GL/gl.h"

namespace openage {
namespace engine {

GLuint load_texture(const char *filename, int *width, int *height);

void draw_texture(int x, int y, GLuint textureid, int width, int height);

} //namespace engine
} //namespace openage

#endif //_TEXTURE_H_
