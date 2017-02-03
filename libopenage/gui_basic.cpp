// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "shader/shader.h"
#include "shader/program.h"

#include "gui_basic.h"
#include "game_singletons_info.h"

#include "util/file.h"

namespace openage {
namespace gui {

GuiBasic::GuiBasic(SDL_Window *window, const std::string &source, qtsdl::GuiSingletonItemsInfo *singleton_items_info, const std::vector<std::string> &search_paths)
	:
	application{},
	render_updater{},
	renderer{window},
	game_logic_updater{},
	image_provider_by_filename{&render_updater, GuiGameSpecImageProvider::Type::ByFilename},
	image_provider_by_graphic_id{&render_updater, GuiGameSpecImageProvider::Type::ByGraphicId},
	image_provider_by_terrain_id{&render_updater, GuiGameSpecImageProvider::Type::ByTerrainId},
	engine{&renderer, {&image_provider_by_filename, &image_provider_by_graphic_id, &image_provider_by_terrain_id}, singleton_items_info},
	subtree{&renderer, &game_logic_updater, &engine, source, search_paths},
	input{&renderer, &game_logic_updater} {

	const char *shader_header_code = "#version 120\n";

	std::string texture_vert_code = util::read_whole_file(static_cast<GameSingletonsInfo*>(singleton_items_info)->data_dir + "/shaders/identity.vert.glsl");
	auto plaintexture_vert = std::make_unique<shader::Shader>(
		GL_VERTEX_SHADER,
		std::initializer_list<const char *>{ shader_header_code, texture_vert_code.c_str() }
	);

	std::string texture_frag_code = util::read_whole_file(static_cast<GameSingletonsInfo*>(singleton_items_info)->data_dir + "/shaders/maptexture.frag.glsl");
	auto plaintexture_frag = std::make_unique<shader::Shader>(
		GL_FRAGMENT_SHADER,
		std::initializer_list<const char *>{ shader_header_code, texture_frag_code.c_str() }
	);

	this->textured_screen_quad_shader = std::make_unique<shader::Program>(&*plaintexture_vert, &*plaintexture_frag);
	this->textured_screen_quad_shader->link();
	this->tex_loc = this->textured_screen_quad_shader->get_uniform_id("texture");
	this->textured_screen_quad_shader->use();
	glUniform1i(this->tex_loc, 0);
	this->textured_screen_quad_shader->stopusing();

	const float screen_quad[] = {
		-1.f, -1.f,
		1.f, -1.f,
		1.f, 1.f,
		-1.f, 1.f,
	};

	glGenBuffers(1, &this->screen_quad_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, this->screen_quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), screen_quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GuiBasic::~GuiBasic() {
	glDeleteBuffers(1, &this->screen_quad_vbo);
}

void GuiBasic::process_events() {
	this->game_logic_updater.process_callbacks();
	this->application.processEvents();
}

bool GuiBasic::on_resize(coord::window new_size) {
	this->renderer.resize(new_size.x, new_size.y);
	return true;
}

bool GuiBasic::on_input(SDL_Event *event) {
	return !this->input.process(event);
}

namespace {
/**
 * Restores blending function.
 */
class BlendPreserver {
public:
	BlendPreserver()
		:
		was_on{},
		src{},
		dst{} {

		glGetBooleanv(GL_BLEND, &this->was_on);

		if (this->was_on != GL_FALSE) {
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &this->src);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &this->dst);
		}
	}

	~BlendPreserver() {
		if (this->was_on != GL_FALSE) {
			glEnable(GL_BLEND);
			glBlendFunc(this->src, this->dst);
		} else {
			glDisable(GL_BLEND);
		}
	}

private:
	GLboolean was_on;
	GLint src;
	GLint dst;
};

}

bool GuiBasic::on_drawhud() {
	this->render_updater.process_callbacks();

	BlendPreserver preserve_blend;

	auto tex = this->renderer.render();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	this->textured_screen_quad_shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glEnableVertexAttribArray(this->textured_screen_quad_shader->pos_id);

	glBindBuffer(GL_ARRAY_BUFFER, this->screen_quad_vbo);
	glVertexAttribPointer(this->textured_screen_quad_shader->pos_id, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(this->textured_screen_quad_shader->pos_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	this->textured_screen_quad_shader->stopusing();

	return true;
}

}} // namespace openage::gui
