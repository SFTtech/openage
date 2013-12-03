#include "callbacks.h"

#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../coord/camhud.h"
#include "../engine.h" 
#include "console.h"

namespace engine {
namespace console {

bool on_engine_tick() {
        if(!visible) {
                return true;
        }

	//TODO handle stuff such as cursor blinking,
	//repeating held-down keys (from engine::input::keystate)
	return true;
}

bool draw() {
        if(!visible) {
                return true;
        }

	glPushMatrix();
	{
		bgcol.use();
		glBegin(GL_QUADS);
		{
			glVertex2i(topleft.x, topleft.y);
			glVertex2i(bottomright.x, topleft.y);
			glVertex2i(bottomright.x, bottomright.y);
			glVertex2i(topleft.x, bottomright.y);
		}
		glEnd();

		fgcol.use();
		glTranslatef(topleft.x + 10, bottomright.y + 10, 0);

		coord::camhud pos {0, 0};

		//render all messages
		for (auto msg = messages.crbegin(); msg != messages.crend(); ++msg) {
			const char *cmsg = *msg;

			//get the font metrics
			FTBBox bbox = font->internal_font->BBox(cmsg);

			FTPoint lower = bbox.Lower();
			FTPoint upper = bbox.Upper();

			//TODO: linewrap if text too wide
			//float w = upper.Xf() - lower.Xf();
			float h = upper.Yf() - lower.Yf();

			//stop displaying older messages no longer fitting on screen
			if ((pos.y + h + spacing) > (topleft.y - bottomright.y)) {
				break;
			}

			font->render_static(pos.x, pos.y, cmsg);

			pos.y += spacing + h;
		}
	}
	glPopMatrix();

	return true;
}

bool handle_inputs(SDL_Event *e) {
	if ((e->type == SDL_KEYDOWN) && (((SDL_KeyboardEvent *) e)->keysym.sym == SDLK_BACKQUOTE)) {
		visible = !visible;
	}

	//only handle inputs if the console is visible
	if(!visible) {
		return true;
	}

	switch (e->type) {
	case SDL_KEYDOWN:
		//TODO handle key inputs

		//do not allow anyone else to handle this input
		return false;
		break;
	}

	return true;
}

bool on_window_resize() {
	console::topleft = {50, (coord::pixel_t) (window_size.y - 50)};
	console::bottomright = {(coord::pixel_t) (window_size.x - 50), 50};
	return true;
}

} //namespace console
} //namespace engine
