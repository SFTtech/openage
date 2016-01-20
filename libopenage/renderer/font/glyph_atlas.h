// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "font.h"

namespace openage {
namespace renderer {

/**
 * A glyph atlas is used to pack and manage several font glyphs in to a single OpenGL texture.
 *
 * A single glyph atlas can be used to stored glyphs from multiple fonts.
 *
 * Currently, the glyph atlas uses a naive "Shelf First-Fit" algorithm based on the article
 * "A Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional Rectangle Bin Packing"
 * by Jukka Jylänki. The article can be found at http://clb.demon.fi/files/RectangleBinPack.pdf
 *
 * This is a very basic algorithm. If there is a need for a more complex packing requirement, the
 * glyph atlas can be easily modified.
 */
class GlyphAtlas {

public:
	/**
	 * Datastructure for a single atlas entry
	 */
	class Entry {
	public:
		Glyph glyph; //!< The Glyph.
		float u0;    //!< The bottom-left texture coordinate in u-axis.
		float v0;    //!< The bottom-left texture coordinate in v-axis.
		float u1;    //!< The top-right texture coordinate in u-axis.
		float v1;    //!< The top-right texture coordinate in v-axis.
	};

public:
	/**
	 * Creates a glyph atlas with the specified width and height.
	 *
	 * Also, creates a OpenGL texture of the same width and height. The contents of this
	 * glyph atlas is automatically synchronized to the texture (when you bind the texture).
	 *
	 * @param width: The width of glyph atlas
	 * @param height: The height of glyph atlas
	 */
	GlyphAtlas(int width = 1024, int height = 1024);

	virtual ~GlyphAtlas();

	/**
	 * Binds the OpenGL texture of this glyph atlas at the specified unit.
	 *
	 * @param unit: the texture unit.
	 */
	void bind(int unit = 0);

	/**
	 * Retrieves the atlas entry for a specified font and glyph.
	 *
	 * If the particular entry does not exist in the atlas, the glyph atlas requests the font to provide
	 * the glyph info. The provided info along with the glyph's bitmap data is used to create a new
	 * cached entry. This entry is then returned.
	 *
	 * @param font: The font
	 * @param codepoint: The glyph whose atlas entry must be retrieved
	 * @returns The atlas entry.
	 */
	GlyphAtlas::Entry get(Font *font, codepoint_t codepoint);

private:
	size_t get_cache_key(Font *font, codepoint_t codepoint) const;

	GlyphAtlas::Entry set(size_t key, const Glyph &glyph, const unsigned char *image);

	void update_dirty_area(int x, int y, int width, int height);

private:
	class Shelf {
	public:
		Shelf(int position, int width, int height);

		bool check_fits(int required_width, int required_height);

		int reserve(int required_width, int required_height);

	private:
		friend class GlyphAtlas;

		int y_position;
		int width;
		int height;
		int remaining_width;
	};

	struct dirty_rect {
		int x1; // Bottom-left x-coord
		int y1; // Bottom-left y-coord
		int x2; // Top-right y-coord
		int y2; // Top-right y-coord
	};

	// The width of the glyph atlas
	int width;

	// The height of the glyph atlas
	int height;

	// Flag indicating if any part of the glyph atlas was updated after previous flush to OpenGL texture
	bool is_dirty;

	// The area in the glyph atlas that was updated after the previous flush to OpenGL texture
	// This is used in optimizing the amount of data pushed to the texture
	dirty_rect dirty_area;

	// The bitmap image data of all glyphs
	std::unique_ptr<unsigned char> buffer;

	// The OpenGL texture handle
	unsigned int texture_id;

	// Cache of all entries stored in this glyph atlas.
	// A combination of font and the glyph's codepoint is used as the cache key.
	std::unordered_map<size_t, GlyphAtlas::Entry> glyphs;

	// List of shelves currently used in the atlas
	std::vector<GlyphAtlas::Shelf> shelves;

};

}} // openage::renderer
