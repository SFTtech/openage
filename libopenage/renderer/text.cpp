// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "text.h"

#include <algorithm>

#include <harfbuzz/hb.h>

#include "../util/strings.h"
#include "font/font.h"

namespace openage {

namespace texturefont_shader {
shader::Program *program;
GLint texture, color, tex_coord;
} // namespace texture_shader

namespace renderer {

struct text_render_vertex {
	float x;
	float y;
	float u;
	float v;

	text_render_vertex()
		:
		text_render_vertex{0.0f, 0.0f, 0.0f, 0.0f} {
	}

	text_render_vertex(float x, float y, float u, float v)
		:
		x{x},
		y{y},
		u{u},
		v{v} {
	}
};

struct text_render_task {
	GLenum mode;
	Color color;
	unsigned int num_elements;
	unsigned int offset;
};

TextRenderer::TextRenderer()
	:
	current_font{nullptr},
	current_color{255, 255, 255, 255},
	is_dirty{true},
	vbo{0},
	ibo{0} {

	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ibo);
}

TextRenderer::~TextRenderer() {
	if (this->vbo) {
		glDeleteBuffers(1, &this->vbo);
	}
	if (this->ibo) {
		glDeleteBuffers(1, &this->ibo);
	}
}

void TextRenderer::set_font(Font *font) {
	if (this->current_font == font) {
		return;
	}

	this->current_font = font;
	this->is_dirty = true;
}

void TextRenderer::set_color(const Color &color) {
	if (this->current_color == color) {
		return;
	}

	this->current_color = color;
	this->is_dirty = true;
}

void TextRenderer::draw(coord::window position, const char *format, ...) {
	std::string text;
	va_list vl;
	va_start(vl, format);
	util::vsformat(format, vl, text);
	va_end(vl);

	this->draw(position.x, position.y, text);
}

void TextRenderer::draw(coord::window position, const std::string &text) {
	this->draw(position.x, position.y, text);
}

void TextRenderer::draw(int x, int y, const std::string &text) {
	if (this->is_dirty || this->render_batches.empty()) {
		this->render_batches.emplace_back(this->current_font, this->current_color);
		this->is_dirty = false;
	}

	this->render_batches.back().passes.emplace_back(x, y, text);
}

void TextRenderer::render() {
	// Sort the batches by font
	std::sort(std::begin(this->render_batches), std::end(this->render_batches),
	          [](const text_render_batch &a, const text_render_batch &b) -> bool {
	              return a.font < b.font;
	          });

	// Merge consecutive batches if font and color values are same
	for (auto current_batch = std::begin(this->render_batches); current_batch != std::end(this->render_batches); ) {
		auto next_batch = current_batch;
		next_batch++;
		if (next_batch != std::end(this->render_batches) &&
		    current_batch->font == next_batch->font &&
		    current_batch->color == next_batch->color) {
			// Merge the render passes of current and next batches and remove the next batch
			std::move(std::begin(next_batch->passes),
			          std::end(next_batch->passes),
			          std::back_inserter(current_batch->passes));
			this->render_batches.erase(next_batch);
		} else {
			current_batch++;
		}
	}

	size_t index = 0;
	std::vector<text_render_vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<text_render_task> render_tasks;
	render_tasks.reserve(this->render_batches.size());
	unsigned int offset = 0;

	// Compute vertices and indices
	for (auto &batch : this->render_batches) {
		Font *font = batch.font;

		unsigned int num_elements = 0;

		for (auto &pass : batch.passes) {
			float x = static_cast<float>(pass.x);
			float y = static_cast<float>(pass.y);

			std::vector<codepoint_t> glyphs = font->get_glyphs(pass.text);

			// We will create 4 vertices & 6 indices for each glyph (GL_TRIANGLES)
			vertices.resize(vertices.size() + glyphs.size() * 4);
			indices.resize(indices.size() + glyphs.size() * 6);

			codepoint_t previous_glyph = 0;
			for (codepoint_t glyph : glyphs) {
				GlyphAtlas::Entry entry = this->glyph_atlas.get(font, glyph);

				float x0 = x + entry.glyph.x_offset;
				float y0 = y + entry.glyph.y_offset - entry.glyph.height;
				float x1 = x0 + entry.glyph.width;
				float y1 = y0 + entry.glyph.height;

				vertices[index*4 + 0] = {x0, y0, entry.u0, entry.v0};
				vertices[index*4 + 1] = {x0, y1, entry.u0, entry.v1};
				vertices[index*4 + 2] = {x1, y1, entry.u1, entry.v1};
				vertices[index*4 + 3] = {x1, y0, entry.u1, entry.v0};

				indices[index*6 + 0] = index*4 + 0;
				indices[index*6 + 1] = index*4 + 1;
				indices[index*6 + 2] = index*4 + 2;
				indices[index*6 + 3] = index*4 + 2;
				indices[index*6 + 4] = index*4 + 3;
				indices[index*6 + 5] = index*4 + 0;

				// Advance the pen position
				x += entry.glyph.x_advance;
				y += entry.glyph.y_advance;

				// Handle font kerning
				if (previous_glyph != 0) {
					x += font->get_horizontal_kerning(previous_glyph, glyph);
				}

				index++;
				num_elements += 6;
				previous_glyph = glyph;
			}
		}

		text_render_task render_task{GL_TRIANGLES, batch.color, num_elements, offset};
		render_tasks.push_back(render_task);

		offset += num_elements;
	}

	// Upload vertices and indices
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(text_render_vertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	texturefont_shader::program->use();

	this->glyph_atlas.bind(0);

	glEnableVertexAttribArray(texturefont_shader::program->pos_id);
	glEnableVertexAttribArray(texturefont_shader::tex_coord);

	glVertexAttribPointer(texturefont_shader::program->pos_id, 2, GL_FLOAT, GL_FALSE,
		sizeof(text_render_vertex), (GLvoid *) offsetof(text_render_vertex, x));
	glVertexAttribPointer(texturefont_shader::tex_coord, 2, GL_FLOAT, GL_FALSE,
		sizeof(text_render_vertex), (GLvoid *) offsetof(text_render_vertex, u));

	for (auto &task : render_tasks) {
		glUniform4f(texturefont_shader::color,
			task.color.r/255.f, task.color.g/255.f, task.color.b/255.f, task.color.a/255.f);
		glDrawElements(task.mode, task.num_elements, GL_UNSIGNED_INT, (GLvoid *) (task.offset * sizeof(unsigned int)));
	}

	glDisableVertexAttribArray(texturefont_shader::program->pos_id);
	glDisableVertexAttribArray(texturefont_shader::tex_coord);

	texturefont_shader::program->stopusing();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Clear the render batches for next frame
	this->render_batches.clear();
}

}} // openage::renderer
