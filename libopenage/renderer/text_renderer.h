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

	/**
	 * Set the font to be used for the future text draw calls.
	 *
	 * @param font: the font to be used.
	 */
	void set_font(Font *font);

	/**
	 * Set the color to be used for the future text draw calls.
	 *
	 * @param color: the color to be used.
	 */
	void set_color(const Color &color);

	/**
	 * Draw a formatted string at the specified position.
	 *
	 * @param position: where the text should be displayed.
	 * @param format: the text format
	 */
	void draw(coord::window position, const char *format, ...);

	/**
	 * Draw text at the specified position.
	 *
	 * @param position: where the text should be displayed.
	 * @param text: the text to be displayed.
	 */
	void draw(coord::window position, const std::string &text);

	/**
	 * Draw text at the specified position.
	 *
	 * @param x: the position in x-direction.
	 * @param y: the position in y-direction.
	 * @param text: the text to be displayed.
	 */
	void draw(int x, int y, const std::string &text);

	/**
	 * Render all the text draw requests made during the frame.
	 */
	void render();

private:
	/**
	 * A single text draw request containing the text and position.
	 */
	struct text_render_batch_pass {
		int x;
		int y;
		std::string text;

		text_render_batch_pass(int x, int y, const std::string &text)
			:
			x{x},
			y{y},
			text{text} {
		}
	};

	/**
	 * The set of text draw requests with the same font and color.
	 */
	struct text_render_batch {
		openage::Font *font;
		Color color;
		std::vector<text_render_batch_pass> passes;

		text_render_batch(openage::Font *font, const Color &color)
			:
			font{font},
			color{color} {
		}
	};

	Font *current_font;
	Color current_color;
	bool is_dirty;
	std::vector<text_render_batch> render_batches;

};

}} // openage::renderer

#endif
