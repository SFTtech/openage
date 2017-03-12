// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <SDL2/SDL_image.h>

#include <memory>
#include <tuple>
#include <vector>

#include "../gamedata/texture.gen.h"


namespace openage {
namespace renderer {

/**
 * A texture for rendering graphically in 3d space.
 * Obtained by registering some texture data to the renderer.
 *
 * The class supports subtextures, so that one big texture ("texture atlas")
 * can contain several smaller images. These are the ones actually to be
 * rendered.
 */
class Texture {
protected:
	Texture(Context *ctx);

public:
	virtual ~Texture();

	/**
	 * Return the dimensions of the whole texture bitmap
	 * @returns tuple(width, height)
	 */
	const std::tuple<size_t, size_t> get_size() const;

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
	 * range 0.0 to 1.0, relative to the whole surface size.
	 */
	const std::tuple<float, float, float, float> get_subtexture_coordinates(size_t subid) const;

	/**
	 * Bind the texture to the given texture unit slot id.
	 */
//	virtual void bind_to(int slot) const = 0;


protected:
	/**
	 * Atlas texture positions.
	 */
	std::vector<gamedata::subtexture> subtextures;

	/**
	 * Width and height of this texture.
	 */
	size_t w, h;
};

}}
