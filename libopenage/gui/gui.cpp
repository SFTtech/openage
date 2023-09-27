// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui.h"

#include "../util/path.h"


namespace openage {
namespace gui {


GUI::GUI(/* SDL_Window *window, */
         const std::string &source,
         const std::string &rootdir,
         EngineQMLInfo *info) :
	application{},
	render_updater{},
	renderer{/* window */},
	game_logic_updater{},
	engine{&renderer},
	subtree{
		&renderer,
		&game_logic_updater,
		&engine,
		source,
		rootdir},
	input{&renderer, &game_logic_updater} {
	// info->display->register_resize_action(this);
	// info->display->register_input_action(this);
	// info->display->register_drawhud_action(this);
}

GUI::~GUI() {
}

void GUI::process_events() {
	this->game_logic_updater.process_callbacks();
	this->application.processEvents();
}

// bool GUI::on_resize(coord::viewport_delta new_size) {
// 	this->renderer.resize(new_size.x, new_size.y);
// 	return true;
// }

// bool GUI::on_input(SDL_Event *event) {
// 	return not this->input.process(event);
// }

// namespace {
// /**
//  * Restores blending function.
//  */
// class BlendPreserver {
// public:
// 	BlendPreserver() :
// 		was_on{},
// 		src{},
// 		dst{} {
// 		glGetBooleanv(GL_BLEND, &this->was_on);

// 		if (this->was_on != GL_FALSE) {
// 			glGetIntegerv(GL_BLEND_SRC_ALPHA, &this->src);
// 			glGetIntegerv(GL_BLEND_DST_ALPHA, &this->dst);
// 		}
// 	}

// 	~BlendPreserver() {
// 		if (this->was_on != GL_FALSE) {
// 			glEnable(GL_BLEND);
// 			glBlendFunc(this->src, this->dst);
// 		}
// 		else {
// 			glDisable(GL_BLEND);
// 		}
// 	}

// private:
// 	GLboolean was_on;
// 	GLint src;
// 	GLint dst;
// };

// } // namespace

// bool GUI::on_drawhud() {
// 	this->render_updater.process_callbacks();

// 	BlendPreserver preserve_blend;

// 	auto tex = this->renderer.render();

// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

// 	this->textured_screen_quad_shader->use();

// 	glActiveTexture(GL_TEXTURE0);
// 	glBindTexture(GL_TEXTURE_2D, tex);

// 	glEnableVertexAttribArray(this->textured_screen_quad_shader->pos_id);

// 	glBindBuffer(GL_ARRAY_BUFFER, this->screen_quad_vbo);
// 	glVertexAttribPointer(
// 		this->textured_screen_quad_shader->pos_id,
// 		2,
// 		GL_FLOAT,
// 		GL_FALSE,
// 		2 * sizeof(float),
// 		0);

// 	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

// 	glDisableVertexAttribArray(this->textured_screen_quad_shader->pos_id);
// 	glBindBuffer(GL_ARRAY_BUFFER, 0);

// 	glBindTexture(GL_TEXTURE_2D, 0);

// 	this->textured_screen_quad_shader->stopusing();

// 	return true;
// }

} // namespace gui
} // namespace openage
