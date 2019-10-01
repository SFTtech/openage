// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "glyph_atlas.h"

#include <algorithm>
#include <functional>
#include <typeindex>
#include <cstring>
#include <epoxy/gl.h>

#include "../../error/error.h"
#include "../../util/hash.h"

namespace openage {
namespace renderer {

GlyphAtlas::Shelf::Shelf(int y_position, int width, int height)
	:
	y_position{y_position},
	width{width},
	height{height},
	remaining_width{width} {
	// Empty
}

bool GlyphAtlas::Shelf::check_fits(int required_width, int required_height) {
	if (required_width > this->remaining_width || required_height > this->height) {
		return false;
	}
	return true;
}

int GlyphAtlas::Shelf::reserve(int required_width, int/* required_height*/) {
	int x_position = this->width - this->remaining_width;
	this->remaining_width -= required_width;
	return x_position;
}

GlyphAtlas::GlyphAtlas(int width, int height)
	:
	width{width},
	height{height},
	is_dirty{false},
	dirty_area{width, height, 0, 0},
	texture_id{0} {

	this->buffer = std::unique_ptr<unsigned char>(new unsigned char[this->width * this->height]);
	memset(this->buffer.get(), 0, this->width * this->height);

	glGenTextures(1, &this->texture_id);
	glBindTexture(GL_TEXTURE_2D, this->texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_UNSIGNED_BYTE, this->buffer.get());
}

GlyphAtlas::~GlyphAtlas() {
	if (this->texture_id) {
		glDeleteTextures(1, &this->texture_id);
	}
}

void GlyphAtlas::bind(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	if (this->is_dirty) {
		// We update the entire width of the atlas because of our buffer memory alignment
		glTexSubImage2D(GL_TEXTURE_2D, 0,
		                0, this->dirty_area.y1, this->width, this->dirty_area.y2 - this->dirty_area.y1,
		                GL_RED, GL_UNSIGNED_BYTE, this->buffer.get() + this->width * this->dirty_area.y1);

		this->is_dirty = false;
		this->dirty_area = {this->width, this->height, 0, 0};
	}
}

GlyphAtlas::Entry GlyphAtlas::get(Font *font, codepoint_t codepoint) {
	size_t key = this->get_cache_key(font, codepoint);
	auto it = this->glyphs.find(key);
	if (it != this->glyphs.end()) {
		return it->second;
	}

	Glyph glyph;
	std::unique_ptr<unsigned char[]> image = font->load_glyph(codepoint, glyph);
	return this->set(key, glyph, image.get());
}

size_t GlyphAtlas::get_cache_key(Font *font, codepoint_t codepoint) const {
	size_t hash = std::hash<std::type_index>()(std::type_index(typeid(openage::renderer::GlyphAtlas)));
	hash = openage::util::hash_combine(hash, std::hash<font_description>()(font->description));
	hash = openage::util::hash_combine(hash, std::hash<codepoint_t>()(codepoint));
	return hash;
}

GlyphAtlas::Entry GlyphAtlas::set(size_t key, const Glyph &glyph, const unsigned char *image) {
	// Give the glyphs a 1px padding in the atlas
	int required_width = glyph.width + 1;
	int required_height = glyph.height + 1;

	if (this->shelves.empty()) {
		this->shelves.emplace_back(0, this->width, required_height);
	}

	for (auto &shelf : this->shelves) {
		if (shelf.check_fits(required_width, required_height)) {
			int x_pos = shelf.reserve(required_width, required_height);
			int y_pos = shelf.y_position;

			// Create a new entry and insert it
			GlyphAtlas::Entry entry;
			entry.glyph = glyph;
			entry.u0 = static_cast<float>(x_pos)/this->width;
			entry.v0 = static_cast<float>(y_pos)/this->height;
			entry.u1 = static_cast<float>(x_pos + glyph.width)/this->width;
			entry.v1 = static_cast<float>(y_pos + glyph.height)/this->height;

			for (unsigned int i = 0; i < glyph.height; i++) {
				memcpy(
					this->buffer.get() + ((y_pos + i) * this->width + x_pos),
					image + (i * glyph.width),
					glyph.width * sizeof(unsigned char)
				);
			}
			this->update_dirty_area(x_pos, y_pos, glyph.width, glyph.height);

			this->glyphs.emplace(key, entry);
			return entry;
		}
	}

	GlyphAtlas::Shelf last_shelf = this->shelves.back();
	if (this->height > (last_shelf.y_position + last_shelf.height + required_height)) {
		this->shelves.emplace_back(last_shelf.y_position + last_shelf.height, this->width, required_height);

		return this->set(key, glyph, image);
	}

	// TODO: figure out a clean way to resize the atlas
	throw Error(MSG(err) << "Not enough space in the atlas");
}

void GlyphAtlas::update_dirty_area(int x, int y, int width, int height) {
	this->is_dirty = true;
	this->dirty_area = {
		std::min(this->dirty_area.x1, x),
		std::min(this->dirty_area.y1, y),
		std::max(this->dirty_area.x2, x + width),
		std::max(this->dirty_area.y2, y + height)};
}

}} // openage::renderer
