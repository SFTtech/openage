// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_TEXTURE_H_
#define OPENAGE_RENDERER_TEXTURE_H_

#include <SDL2/SDL_image.h>

#include <memory>
#include <tuple>
#include <vector>

#include "../gamedata/texture.gen.h"

namespace openage {
namespace renderer {


/**
 * Texture format, used for setting pixel data size.
 */
enum class texture_format {
	rgb,
	rgba,
};


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
	Texture() = default;
public:
	virtual ~Texture() {};

	/**
	 * Return the dimensions of the whole texture bitmap
	 * @returns tuple(width, height)
	 */
	virtual const std::tuple<size_t, size_t> get_size() const;

	/**
	 * Get the subtexture coordinates by its idea.
	 */
	virtual const gamedata::subtexture *get_subtexture(size_t subid) const;

	/**
	 * @return the number of available subtextures
	 */
	virtual int get_subtexture_count() const;

	/**
	 * Fetch the size of the given subtexture.
	 * @param subid: index of the requested subtexture
	 */
	virtual const std::tuple<int, int> get_subtexture_size(size_t subid) const;

	/**
	 * get atlas subtexture coordinates.
	 *
	 * @returns left, right, top and bottom bounds as coordinates these pick
	 * the requested area out of the big texture. returned as floats in
	 * range 0.0 to 1.0, relative to the whole surface size.
	 */
	virtual const std::tuple<float, float, float, float> get_subtexture_coordinates(size_t subid) const;

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


/**
 * Data for textures. Just used for transmitting the data to the GPU.
 */
class TextureData {
protected:
	TextureData() = default;

public:
	/**
	 * Create a texture from a rgba8 array.
	 * It will have w * h * 4byte storage.
	 * Each pixel has one byte, and r g b and alpha values.
	 */
	TextureData(int width, int height, uint8_t *data);

	virtual ~TextureData() = default;

	/**
	 * The data format of the texture.
	 */
	texture_format format;

	/**
	 * Width and height of this texture.
	 */
	int w, h;

	/**
	 * Raw texture pixel data.
	 * r g b a values, each 8 bit.
	 */
	std::unique_ptr<uint8_t[]> data;

	/**
	 * The atlas texture positions.
	 */
	std::vector<gamedata::subtexture> subtextures;
};


/**
 * Create a texture from an image file.
 *
 * Uses SDL Image internally.
 */
class FileTextureData : public TextureData {
public:
	/**
	 * Create a texture from a existing image file.
	 *
	 * For supported image file types, see the SDL_Image initialization in
	 * the engine.
	 */
	FileTextureData(const std::string &filename, bool use_metafile=false);
	~FileTextureData() = default;

protected:
	/**
	 * Use the meta information file providing info about
	 * texture atlas positions.
	 */
	bool use_metafile;

	/**
	 * File system path name of
	 */
	std::string filename;
};

}} // openage::renderer

#endif
