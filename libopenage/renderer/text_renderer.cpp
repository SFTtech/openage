// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "text_renderer.h"

#include <algorithm>
#include <epoxy/gl.h>

#include "../log/log.h"
#include "../util/strings.h"
#include "../font.h"

namespace openage {
namespace renderer {

TextRenderer::TextRenderer()
	:
	current_font{nullptr},
	current_color{255, 255, 255, 255},
	is_dirty{true} {
	// Empty
}

TextRenderer::~TextRenderer() {
	// Empty
}

void TextRenderer::set_font(openage::Font *font) {
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

	// Render all the batches
	for (auto &batch : this->render_batches) {
		glColor4f(batch.color.r / 255.f,
		          batch.color.g / 255.f,
		          batch.color.b / 255.f,
		          batch.color.a / 255.f);
		for (auto &pass : batch.passes) {
			batch.font->render_static(pass.x, pass.y, pass.text.c_str());
		}
	}

	// Clear the render batches for next frame
	this->render_batches.clear();
}

}} // openage::renderer
