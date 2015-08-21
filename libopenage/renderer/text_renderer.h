// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TEXT_RENDERER_H_
#define OPENAGE_RENDERER_TEXT_RENDERER_H_

#include <vector>
#include <string>

#include "../coord/window.h"
#include "color.h"

namespace openage {

class Font;

namespace renderer {

class TextRenderer {

public:
	TextRenderer();

	virtual ~TextRenderer();

	void set_font(Font *font);
	void set_color(const Color &color);

	void draw(coord::window position, const char *format, ...);
	void draw(coord::window position, const std::string &text);
	void draw(int x, int y, const std::string &text);

	void render();

private:
	struct TextRenderBatchPass {
		int x;
		int y;
		std::string text;
	};

	struct TextRenderBatch {
		openage::Font *font;
		Color color;
		std::vector<TextRenderBatchPass> passes;
	};

	Font *current_font;
	Color current_color;
	bool is_dirty;
	std::vector<TextRenderBatch> render_batches;

};

}} // openage::renderer

#endif
