// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cmath>
#include <cstdio>

#include "log/log.h"
#include "error/error.h"
#include "util/file.h"

namespace openage {

// TODO: remove these global variables!!!
// definition of the shaders,
// they are "external" in the header.
namespace texture_shader {
shader::Program *program;
GLint texture, tex_coord;
}

namespace teamcolor_shader {
shader::Program *program;
GLint texture, tex_coord;
GLint player_id_var, alpha_marker_var, player_color_var;
}

namespace alphamask_shader {
shader::Program *program;
GLint base_texture, mask_texture, base_coord, mask_coord, show_mask;
}

Texture::Texture(int width, int height, std::unique_ptr<uint32_t[]> data)
	:
	use_metafile{false} {
	ENSURE(glGenBuffers != nullptr, "gl not initialized properly");

	this->w = width;
	this->h = height;
	this->buffer = std::make_unique<gl_texture_buffer>();
	this->buffer->transferred = false;
	this->buffer->texture_format_in = GL_RGBA8;
	this->buffer->texture_format_out = GL_RGBA;
	this->buffer->data = std::move(data);
	this->subtextures.push_back({0, 0, this->w, this->h, this->w/2, this->h/2});
}

Texture::Texture(const std::string &filename, bool use_metafile)
	:
	use_metafile{use_metafile},
	filename{filename} {

	// load the texture upon creation
	this->load();
}

void Texture::load() {
	SDL_Surface *surface;
	surface = IMG_Load(this->filename.c_str());

	if (!surface) {
		throw Error(MSG(err) <<
			"Could not load texture from " <<
			filename << ": " << IMG_GetError());
	} else {
		log::log(MSG(dbg) << "Texture has been loaded from " << filename);
	}

	this->buffer = std::make_unique<gl_texture_buffer>();

	// glTexImage2D format determination
	switch (surface->format->BytesPerPixel) {
	case 3: // RGB 24 bit
		this->buffer->texture_format_in  = GL_RGB8;
		this->buffer->texture_format_out = GL_RGB;
		break;
	case 4: // RGBA 32 bit
		this->buffer->texture_format_in  = GL_RGBA8;
		this->buffer->texture_format_out = GL_RGBA;
		break;
	default:
		throw Error(MSG(err) <<
			"Unknown texture bit depth for " << filename << ": " <<
			surface->format->BytesPerPixel << " bytes per pixel");

		break;
	}
	this->w = surface->w;
	this->h = surface->h;

	// temporary buffer for pixel data
	this->buffer->transferred = false;
	this->buffer->data = std::make_unique<uint32_t[]>(this->w * this->h);
	memcpy(
		this->buffer->data.get(),
		surface->pixels,
		this->w * this->h *
		surface->format->BytesPerPixel
	);
	SDL_FreeSurface(surface);

	if (use_metafile) {
		// change the suffix to .docx (lol)
		size_t m_len = filename.length() + 2;
		char *meta_filename = new char[m_len];
		strncpy(meta_filename, filename.c_str(), m_len - 5);
		strncpy(meta_filename + m_len - 5, "docx", 5);

		log::log(MSG(info) << "Loading meta file: " << meta_filename);

		// get subtexture information by meta file exported by script
		util::read_csv_file(meta_filename, this->subtextures);

		// TODO: use information from empires.dat for that, also use x and y sizes:
		this->atlas_dimensions = sqrt(this->subtextures.size());
		delete[] meta_filename;
	}
	else {
		// we don't have a texture description file.
		// use the whole image as one texture then.
		gamedata::subtexture s{0, 0, this->w, this->h, this->w/2, this->h/2};

		this->subtextures.push_back(s);
	}
}

GLuint Texture::make_gl_texture(int iformat, int oformat, int w, int h, void *data) const {
	// generate 1 texture handle
	GLuint textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);

	// sdl surface -> opengl texture
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		iformat, w, h, 0,
		oformat, GL_UNSIGNED_BYTE, data
	);

	// later drawing settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureid;
}

void Texture::main_thread_load() const {
	if (!this->buffer->transferred) {
		this->buffer->id = this->make_gl_texture(
			this->buffer->texture_format_in,
			this->buffer->texture_format_out,
			this->w,
			this->h,
			this->buffer->data.get()
		);
		this->buffer->data = nullptr;
		glGenBuffers(1, &this->buffer->vertbuf);
		this->buffer->transferred = true;
	}
}

void Texture::unload() {
	glDeleteTextures(1, &this->buffer->id);
	glDeleteBuffers(1, &this->buffer->vertbuf);
}


void Texture::reload() {
	this->unload();
	this->load();
}


Texture::~Texture() {
	this->unload();
}


void Texture::fix_hotspots(unsigned x, unsigned y) {
	for (auto &subtexture : this->subtextures) {
		subtexture.cx = x;
		subtexture.cy = y;
	}
}


void Texture::draw(coord::camhud pos, unsigned int mode, bool mirrored, int subid, unsigned player) const {
	this->draw(pos.x, pos.y, mode, mirrored, subid, player, nullptr, -1);
}


void Texture::draw(coord::camgame pos, unsigned int mode,  bool mirrored, int subid, unsigned player) const {
	this->draw(pos.x, pos.y, mode, mirrored, subid, player, nullptr, -1);
}


void Texture::draw(coord::tile pos, unsigned int mode, int subid, Texture *alpha_texture, int alpha_subid) const {
	coord::camgame draw_pos = pos.to_tile3().to_phys3().to_camgame();
	this->draw(draw_pos.x, draw_pos.y, mode, false, subid, 0, alpha_texture, alpha_subid);
}


void Texture::draw(coord::pixel_t x, coord::pixel_t y,
                   unsigned int mode, bool mirrored,
                   int subid, unsigned player,
                   Texture *alpha_texture, int alpha_subid) const {
	this->main_thread_load();
	glColor4f(1, 1, 1, 1);

	bool use_playercolors = false;
	bool use_alphashader = false;
	const gamedata::subtexture *mtx;

	int *pos_id, *texcoord_id, *masktexcoord_id;

	// is this texture drawn with an alpha mask?
	if ((mode & ALPHAMASKED) && alpha_subid >= 0 && alpha_texture != nullptr) {
		alphamask_shader::program->use();

		// bind the alpha mask texture to slot 1
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, alpha_texture->get_texture_id());

		// get the alphamask subtexture (the blend mask!)
		mtx = alpha_texture->get_subtexture(alpha_subid);
		pos_id = &alphamask_shader::program->pos_id;
		texcoord_id = &alphamask_shader::base_coord;
		masktexcoord_id = &alphamask_shader::mask_coord;
		use_alphashader = true;
	}
	// is this texure drawn with replaced pixels for team coloring?
	else if (mode & PLAYERCOLORED) {
		teamcolor_shader::program->use();

		//set the desired player id in the shader
		glUniform1i(teamcolor_shader::player_id_var, player);
		pos_id = &teamcolor_shader::program->pos_id;
		texcoord_id = &teamcolor_shader::tex_coord;
		use_playercolors = true;
	}
	// mkay, we just draw the plain texture otherwise.
	else {
		texture_shader::program->use();
		pos_id = &texture_shader::program->pos_id;
		texcoord_id = &texture_shader::tex_coord;
	}

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->buffer->id);

	const gamedata::subtexture *tx = this->get_subtexture(subid);

	int left, right, top, bottom;

	// coordinates where the texture will be drawn on screen.
	bottom  = y      - (tx->h - tx->cy);
	top     = bottom + tx->h;

	if (not mirrored) {
		left  = x    - tx->cx;
		right = left + tx->w;
	} else {
		left  = x    + tx->cx;
		right = left - tx->w;
	}

	// convert the texture boundaries to float
	// these will be the vertex coordinates.
	float leftf, rightf, topf, bottomf;
	leftf   = (float) left;
	rightf  = (float) right;
	topf    = (float) top;
	bottomf = (float) bottom;

	// subtexture coordinates
	// left, right, top and bottom bounds as coordinates
	// these pick the requested area out of the big texture.
	float txl, txr, txt, txb;
	this->get_subtexture_coordinates(tx, &txl, &txr, &txt, &txb);

	float mtxl=0, mtxr=0, mtxt=0, mtxb=0;
	if (use_alphashader) {
		alpha_texture->get_subtexture_coordinates(mtx, &mtxl, &mtxr, &mtxt, &mtxb);
	}

	// this array will be uploaded to the GPU.
	// it contains all dynamic vertex data (position, tex coordinates, mask coordinates)
	float vdata[] {
		leftf,  topf,
		leftf,  bottomf,
		rightf, bottomf,
		rightf, topf,
		txl,    txt,
		txl,    txb,
		txr,    txb,
		txr,    txt,
		mtxl,   mtxt,
		mtxl,   mtxb,
		mtxr,   mtxb,
		mtxr,   mtxt
	};


	// store vertex buffer data, TODO: prepare this sometime earlier.
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer->vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STREAM_DRAW);

	// enable vertex buffer and bind it to the vertex attribute
	glEnableVertexAttribArray(*pos_id);
	glEnableVertexAttribArray(*texcoord_id);
	if (use_alphashader) {
		glEnableVertexAttribArray(*masktexcoord_id);
	}

	// set data types, offsets in the vdata array
	glVertexAttribPointer(*pos_id,      2, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	glVertexAttribPointer(*texcoord_id, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(float) * 8));
	if (use_alphashader) {
		glVertexAttribPointer(*masktexcoord_id, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(float) * 8 * 2));
	}

	// draw the vertex array
	glDrawArrays(GL_QUADS, 0, 4);

	// unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(*pos_id);
	glDisableVertexAttribArray(*texcoord_id);
	if (use_alphashader) {
		glDisableVertexAttribArray(*masktexcoord_id);
	}

	// disable the shaders.
	if (use_playercolors) {
		teamcolor_shader::program->stopusing();
	} else if (use_alphashader) {
		alphamask_shader::program->stopusing();
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
	} else {
		texture_shader::program->stopusing();
	}

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}


const gamedata::subtexture *Texture::get_subtexture(int subid) const {
	if (subid >= 0 && (static_cast<size_t>(subid) < this->subtextures.size())) {
		return &this->subtextures[subid];
	}
	else {
		throw Error(MSG(err) <<
			"Unknown subtexture requested for texture " <<
			this->filename << ": " << subid);
	}
}


void Texture::get_subtexture_coordinates(int subid, float *txl, float *txr, float *txt, float *txb) const {
	const gamedata::subtexture *tx = this->get_subtexture(subid);
	this->get_subtexture_coordinates(tx, txl, txr, txt, txb);
}


void Texture::get_subtexture_coordinates(const gamedata::subtexture *tx,
                                         float *txl, float *txr, float *txt, float *txb) const {
	*txl = ((float)tx->x)           /this->w;
	*txr = ((float)(tx->x + tx->w)) /this->w;
	*txt = ((float)tx->y)           /this->h;
	*txb = ((float)(tx->y + tx->h)) /this->h;
}


int Texture::get_subtexture_count() const {
	return this->subtextures.size();
}


void Texture::get_subtexture_size(int subid, int *w, int *h) const {
	const gamedata::subtexture *subtex = this->get_subtexture(subid);
	*w = subtex->w;
	*h = subtex->h;
}


GLuint Texture::get_texture_id() const {
	this->main_thread_load();
	return this->buffer->id;
}

} // openage
