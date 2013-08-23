#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "GL/gl.h"

namespace openage {
namespace engine {

class Texture {
public:
	int w;
	int h;

	Texture(const char *filename);
	~Texture();

	void draw(int posx, int posy);
private:
	GLuint gl_id;
};

} //namespace engine
} //namespace openage

#endif //_TEXTURE_H_
