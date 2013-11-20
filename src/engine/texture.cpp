#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <math.h>
#include <stdio.h>

#include "../log/log.h"
#include "../util/error.h"
#include "../util/filetools.h"

namespace openage {
namespace engine {

//real definition of the shaders,
//they are "external" in the header.
namespace texture_shader {
shader::Program *program;
GLint texture, tex_coord;
} //namespace texture_shader

namespace teamcolor_shader {
shader::Program *program;
GLint texture, tex_coord;
GLint player_id_var, alpha_marker_var, player_color_var;
} //namespace teamcolor_shader

namespace alphamask_shader {
shader::Program *program;
GLint base_texture, mask_texture, base_coord, mask_coord, show_mask;
} //namespace alphamask_shader


Texture::Texture(const char *filename, bool use_metafile, unsigned int mode) {

	this->use_player_color_tinting = 0 < (mode & PLAYERCOLORED);
	this->use_alpha_masking        = 0 < (mode & ALPHAMASKED);

	this->alpha_subid = -1;
	this->alpha_texture = nullptr;

	SDL_Surface *surface;
	GLuint textureid;
	int texture_format;

	surface = IMG_Load(filename);

	if (!surface) {
		throw util::Error("Could not load texture from '%s': %s", filename, IMG_GetError());
	}
	else {
		log::dbg1("Loaded texture from '%s'", filename);
	}

	//glTexImage2D format determination
	switch (surface->format->BytesPerPixel) {
	case 3: //RGB 24 bit
		texture_format = GL_RGB;
		break;
	case 4: //RGBA 32 bit
		texture_format = GL_RGBA;
		break;
	default:
		throw util::Error("Unknown texture bit depth for '%s': %d bytes per pixel)", filename, surface->format->BytesPerPixel);
		break;
	}

	this->w = surface->w;
	this->h = surface->h;

	//generate 1 texture handle
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);

	//sdl surface -> opengl texture
	glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

	//later drawing settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	this->id = textureid;

	if (use_metafile) {
		//change the suffix to .docx (lol)
		size_t m_len = strlen(filename) + 2;
		char *meta_filename = new char[m_len];
		strncpy(meta_filename, filename, m_len);

		meta_filename[m_len-5] = 'd';
		meta_filename[m_len-4] = 'o';
		meta_filename[m_len-3] = 'c';
		meta_filename[m_len-2] = 'x';
		meta_filename[m_len-1] = '\0';


		log::msg("loading meta file %s", meta_filename);

		//get subtexture information by meta file exported by script
		char *texture_meta_file = util::read_whole_file(meta_filename);

		delete[] meta_filename;

		char *tmeta_seeker = texture_meta_file;
		char *currentline = texture_meta_file;

		bool wanting_count = true;

		for(; *tmeta_seeker != '\0'; tmeta_seeker++) {

			if (*tmeta_seeker == '\n') {
				*tmeta_seeker = '\0';

				if (*currentline != '#') {
					//count, index, x, y, width, height, hotspotx, hotspoty
					uint n, idx, tx, ty, tw, th, hx, hy;

					if(sscanf(currentline, "n=%u", &n) == 1) {
						this->subtexture_count = n;
						this->atlas_dimensions = sqrt(n);
						this->subtextures = new struct subtexture[n];
						wanting_count = false;
					}
					else {
						if (wanting_count) {
							throw util::Error("texture meta line found, but no count set yet in %s", meta_filename);
						}
						else if(sscanf(currentline, "%u=%u,%u,%u,%u,%u,%u", &idx, &tx, &ty, &tw, &th, &hx, &hy)) {
							struct subtexture subtext;

							//lower left coordinates, origin
							subtext.x = tx;
							subtext.y = ty;

							//width and height from lower left origin
							subtext.w = tw;
							subtext.h = th;

							//hotspot/center coordinates
							subtext.cx = hx;
							subtext.cy = hy;

							this->subtextures[idx] = subtext;
						}
						else {
							throw util::Error("unknown line content reading texture meta file %s", meta_filename);
						}
					}
				}
				currentline = tmeta_seeker + 1;
			}
		}

		delete[] texture_meta_file;
	}
	else { //this texture does not contain subtextures
		struct subtexture subtext;

		subtext.x = 0;
		subtext.y = 0;
		subtext.w = this->w;
		subtext.h = this->h;
		subtext.cx = 0;
		subtext.cy = 0;

		this->subtexture_count = 1;
		this->subtextures = new struct subtexture[1];
		this->subtextures[0] = subtext;
	}
	glGenBuffers(1, &this->vertbuf);
}

Texture::~Texture() {
	delete[] this->subtextures;
	glDeleteTextures(1, &this->id);
}

void Texture::fix_hotspots(unsigned x, unsigned y) {
	for(int i = 0; i < subtexture_count; i++) {
		this->subtextures[i].cx = x;
		this->subtextures[i].cy = y;
	}
}

void Texture::draw(coord::phys pos, bool mirrored, int subid, unsigned player) {
	coord::camera campos = coord::phys_to_camera(pos);
	this->draw(campos.x, campos.y, mirrored, subid, player);
}

void Texture::draw(int x, int y, bool mirrored, int subid, unsigned player) {
	glColor4f(1, 1, 1, 1);

	bool use_playercolors = false;
	bool use_alphashader = false;
	struct subtexture *mtx;

	int *pos_id, *texcoord_id, *masktexcoord_id;

	//is this texture drawn with an alpha mask?
	if (this->alpha_subid >= 0 && this->use_alpha_masking) {
		alphamask_shader::program->use();

		//bind the alpha mask texture to slot 1
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, this->alpha_texture->get_texture_id());

		//get the alphamask subtexture (the blend mask!)
		mtx = this->alpha_texture->get_subtexture(this->alpha_subid);
		pos_id = &alphamask_shader::program->pos_id;
		texcoord_id = &alphamask_shader::base_coord;
		masktexcoord_id = &alphamask_shader::mask_coord;
		use_alphashader = true;
	}
	//is this texure drawn with replaced pixels for team coloring?
	else if (this->use_player_color_tinting) {
		teamcolor_shader::program->use();

		//set the desired player id in the shader
		glUniform1i(teamcolor_shader::player_id_var, player);
		pos_id = &teamcolor_shader::program->pos_id;
		texcoord_id = &teamcolor_shader::tex_coord;
		use_playercolors = true;
	}
	//mkay, we just draw the plain texture otherwise.
	else {
		texture_shader::program->use();
		pos_id = &texture_shader::program->pos_id;
		texcoord_id = &texture_shader::tex_coord;
	}

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->id);

	struct subtexture *tx = this->get_subtexture(subid);

	float left, right, top, bottom;

	//coordinates where the texture will be drawn on screen.
	bottom  = y      - tx->cy;
	top     = bottom + tx->h;

	if (!mirrored) {
		left  = x    - tx->cx;
		right = left + tx->w;
	} else {
		left  = x    + tx->cx;
		right = left - tx->w;
	}

	//subtexture coordinates
	//left, right, top and bottom bounds as coordinates
	//these pick the requested area out of the big texture.
	float txl, txr, txt, txb;
	this->get_subtexture_coordinates(tx, &txl, &txr, &txt, &txb);

	float mtxl, mtxr, mtxt, mtxb;
	if (use_alphashader) {
		this->alpha_texture->get_subtexture_coordinates(mtx, &mtxl, &mtxr, &mtxt, &mtxb);
	}

	float vdata[] {
		left, bottom,
		left, top,
		right, top,
		right, bottom,
		txl, txt,
		txl, txb,
		txr, txb,
		txr, txt,
		mtxl, mtxt,
		mtxl, mtxb,
		mtxr, mtxb,
		mtxr, mtxt
	};


	//store vertex buffer data, TODO: prepare this sometime earlier.
	glBindBuffer(GL_ARRAY_BUFFER, this->vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STREAM_DRAW);

	//enable vertex buffer and bind it to the vertex attribute
	glEnableVertexAttribArray(*pos_id);
	glEnableVertexAttribArray(*texcoord_id);
	if (use_alphashader) {
		glEnableVertexAttribArray(*masktexcoord_id);
	}

	//set data types, offsets in the vdata array
	glVertexAttribPointer(*pos_id,      2, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	glVertexAttribPointer(*texcoord_id, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(float) * 8));
	if (use_alphashader) {
		glVertexAttribPointer(*masktexcoord_id, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(float) * 8 * 2));
	}

	//draw the vertex array
	glDrawArrays(GL_QUADS, 0, 4);

	//unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(*pos_id);
	glDisableVertexAttribArray(*texcoord_id);
	if (use_alphashader) {
		glDisableVertexAttribArray(*masktexcoord_id);
	}

	//disable the shaders.
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


struct subtexture *Texture::get_subtexture(int subid) {
	if (subid < this->subtexture_count && subid >= 0) {
		return &this->subtextures[subid];
	}
	else {
		throw util::Error("requested unknown subtexture %d", subid);
	}
}


/**
get atlas subtexture coordinates.

left, right, top and bottom bounds as coordinates
these pick the requested area out of the big texture.
returned as floats in range 0.0 to 1.0
*/
void Texture::get_subtexture_coordinates(int subid, float *txl, float *txr, float *txt, float *txb) {
	struct subtexture *tx = this->get_subtexture(subid);
	this->get_subtexture_coordinates(tx, txl, txr, txt, txb);
}

void Texture::get_subtexture_coordinates(struct subtexture *tx, float *txl, float *txr, float *txt, float *txb) {
	*txl = ((float)tx->x)           /this->w;
	*txr = ((float)(tx->x + tx->w)) /this->w;
	*txt = ((float)tx->y)           /this->h;
	*txb = ((float)(tx->y + tx->h)) /this->h;
}


int Texture::get_subtexture_count() {
	return this->subtexture_count;
}

void Texture::get_subtexture_size(int subid, int *w, int *h) {
	struct subtexture *subtex = this->get_subtexture(subid);
	*w = subtex->w;
	*h = subtex->h;
}

void Texture::activate_alphamask(Texture *mask, int subid) {
	this->alpha_texture = mask;
	this->alpha_subid = subid;
}

void Texture::disable_alphamask() {
	this->alpha_texture = nullptr;
	this->alpha_subid = -1;
}

GLuint Texture::get_texture_id() {
	return this->id;
}


} //namespace engine
} //namespace openage
