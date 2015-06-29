// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TEXTURE_H_
#define OPENAGE_RENDERER_TEXTURE_H_

#include <memory>
#include <tuple>
#include <vector>

#include "../gamedata/texture.gen.h"

namespace openage {
namespace renderer {

/**
 * A texture for rendering graphically.
 *
 * You may believe it or not, but this class represents a single texture,
 * which can be drawn on the screen.
 *
 * The class supports subtextures, so that one big texture ("texture atlas")
 * can contain several smaller images. These are the ones actually to be
 * rendered.
 */
class Texture {
public:
	virtual ~Texture() {};

	/**
	 * Return the dimensions of the whole texture bitmap
	 * @returns tuple(width, height)
	 */
	std::tuple<size_t, size_t> get_size() const;

	/**
	 * Get the subtexture coordinates by its idea.
	 */
	const gamedata::subtexture *get_subtexture(size_t subid) const;

	/**
	 * @return the number of available subtextures
	 */
	int get_subtexture_count() const;

	/**
	 * Fetch the size of the given subtexture.
	 * @param subid: index of the requested subtexture
	 */
	const std::tuple<int, int> get_subtexture_size(size_t subid) const;

	/**
	 * get atlas subtexture coordinates.
	 *
	 * @returns left, right, top and bottom bounds as coordinates these pick
	 * the requested area out of the big texture. returned as floats in
	 * range 0.0 to 1.0, relative to the whole bitmap size.
	 */
	const std::tuple<float, float, float, float> get_subtexture_coordinates(size_t subid) const;

protected:
	std::vector<gamedata::subtexture>subtextures;
	size_t w, h;
};


class RawTexture : public Texture {
public:
	/**
	 * Create a texture from a rgba8 array.
	 * It will have w * h * 4byte storage.
	 * Each pixel has one byte, and r g b and alpha values.
	 */
	RawTexture(int width, int height, std::unique_ptr<uint32_t[]> data);
};


class FileTexture : public Texture {
public:
	/**
	 * Create a texture from a existing image file.
	 *
	 * For supported image file types, see the SDL_Image initialization in
	 * the engine.
	 */
	FileTexture(const std::string &filename, bool use_metafile=false);
private:
	bool use_metafile;
	std::string filename;
};

}} // namespace openage::renderer

#endif
