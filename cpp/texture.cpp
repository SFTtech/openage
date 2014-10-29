#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <math.h>
#include <stdio.h>

#include "log.h"
#include "util/error.h"
#include "util/file.h"

#include "renderer/renderer.h"

using namespace openage::graphics;

namespace openage {

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


Texture::Texture(std::string filename, bool use_metafile, unsigned int mode) {

	this->use_player_color_tinting = 0 < (mode & PLAYERCOLORED);
	this->use_alpha_masking        = 0 < (mode & ALPHAMASKED);

	SDL_Surface *surface;
	GLuint textureid;
	int texture_format_in;
	int texture_format_out;

	surface = IMG_Load(filename.c_str());

	if (!surface) {
		throw util::Error("Could not load texture from '%s': %s", filename.c_str(), IMG_GetError());
	}
	else {
		log::dbg1("Loaded texture from '%s'", filename.c_str());
	}

	//glTexImage2D format determination
	switch (surface->format->BytesPerPixel) {
	case 3: //RGB 24 bit
		texture_format_in  = GL_RGB8;
		texture_format_out = GL_RGB;
		break;
	case 4: //RGBA 32 bit
		texture_format_in  = GL_RGBA8;
		texture_format_out = GL_RGBA;
		break;
	default:
		throw util::Error("Unknown texture bit depth for '%s': %d bytes per pixel)", filename.c_str(), surface->format->BytesPerPixel);
		break;
	}

	this->w = surface->w;
	this->h = surface->h;

	//generate 1 texture handle
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);

	// sdl surface -> opengl texture
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		texture_format_in, surface->w, surface->h, 0,
		texture_format_out, GL_UNSIGNED_BYTE, surface->pixels
	);

	//later drawing settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	this->id = textureid;

	if (use_metafile) {
		//change the suffix to .docx (lol)
		size_t m_len = filename.length() + 2;
		char *meta_filename = new char[m_len];
		strncpy(meta_filename, filename.c_str(), m_len);

		meta_filename[m_len-5] = 'd';
		meta_filename[m_len-4] = 'o';
		meta_filename[m_len-3] = 'c';
		meta_filename[m_len-2] = 'x';
		meta_filename[m_len-1] = '\0';


		log::msg("loading meta file %s", meta_filename);

		//get subtexture information by meta file exported by script
		this->subtextures = util::read_csv_file<gamedata::subtexture>(meta_filename);
		this->subtexture_count = this->subtextures.size();

		//TODO: use information from empires.dat for that, also use x and y sizes:
		this->atlas_dimensions = sqrt(this->subtexture_count);
		delete[] meta_filename;
	}
	else {
		// we don't have a texture description file.
		// use the whole image as one texture then.
		struct gamedata::subtexture s {0, 0, this->w, this->h, this->w/2, this->h/2};

		this->subtexture_count = 1;
		this->subtextures.push_back(s);
	}
	glGenBuffers(1, &this->vertbuf);
}

Texture::~Texture() {
	glDeleteTextures(1, &this->id);
	glDeleteBuffers(1, &this->vertbuf);
}

void Texture::fix_hotspots(unsigned x, unsigned y) {
	for(size_t i = 0; i < subtexture_count; i++) {
		this->subtextures[i].cx = x;
		this->subtextures[i].cy = y;
	}
}

void Texture::draw(coord::camhud pos, bool mirrored, int subid, unsigned player) {
	this->draw(pos.x, pos.y, mirrored, subid, player, nullptr, -1);
}

void Texture::draw(coord::camgame pos, bool mirrored, int subid, unsigned player) {
	this->draw(pos.x, pos.y, mirrored, subid, player, nullptr, -1);
}

void Texture::draw(coord::tile pos, int subid, Texture *alpha_texture, int alpha_subid) {
	coord::camgame draw_pos = pos.to_tile3().to_phys3().to_camgame();
	this->draw(draw_pos.x, draw_pos.y, false, subid, 0, alpha_texture, alpha_subid);
}

void Texture::draw(coord::pixel_t x, coord::pixel_t y, bool mirrored, int subid, unsigned player, Texture *alpha_texture, int alpha_subid) {
	glColor4f(1, 1, 1, 1);

	//log::dbg("drawing texture at %hd, %hd", x, y);
	eMaterialType::Enum cType; //Use this to help transition to new renderer
	
	bool use_playercolors = false;
	bool use_alphashader = false;
	struct gamedata::subtexture *mtx;

	int *pos_id, *texcoord_id, *masktexcoord_id;

	//is this texture drawn with an alpha mask?
	if (this->use_alpha_masking && alpha_subid >= 0 && alpha_texture != nullptr) {
		alphamask_shader::program->use();

		//bind the alpha mask texture to slot 1
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, alpha_texture->get_texture_id());

		//get the alphamask subtexture (the blend mask!)
		mtx = alpha_texture->get_subtexture(alpha_subid);
		pos_id = &alphamask_shader::program->pos_id;
		texcoord_id = &alphamask_shader::base_coord;
		masktexcoord_id = &alphamask_shader::mask_coord;
		use_alphashader = true;
		
		cType = eMaterialType::keAlphaMask;
	}
	//is this texure drawn with replaced pixels for team coloring?
	else if (this->use_player_color_tinting) {
		teamcolor_shader::program->use();

		//set the desired player id in the shader
		glUniform1i(teamcolor_shader::player_id_var, player);
		pos_id = &teamcolor_shader::program->pos_id;
		texcoord_id = &teamcolor_shader::tex_coord;
		use_playercolors = true;
		
		cType = eMaterialType::keColorReplace;
	}
	//mkay, we just draw the plain texture otherwise.
	else {
		texture_shader::program->use();
		pos_id = &texture_shader::program->pos_id;
		texcoord_id = &texture_shader::tex_coord;
		
		cType = eMaterialType::keNormal;
	}

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->id);

	struct gamedata::subtexture *tx = this->get_subtexture(subid);

	int left, right, top, bottom;

	//coordinates where the texture will be drawn on screen.
	bottom  = y      - (tx->h - tx->cy);
	top     = bottom + tx->h;

	if (not mirrored) {
		left  = x    - tx->cx;
		right = left + tx->w;
	} else {
		left  = x    + tx->cx;
		right = left - tx->w;
	}

	//convert the texture boundaries to float
	//these will be the vertex coordinates.
	float leftf, rightf, topf, bottomf;
	leftf   = (float) left;
	rightf  = (float) right;
	topf    = (float) top;
	bottomf = (float) bottom;

	//subtexture coordinates
	//left, right, top and bottom bounds as coordinates
	//these pick the requested area out of the big texture.
	float txl, txr, txt, txb;
	this->get_subtexture_coordinates(tx, &txl, &txr, &txt, &txb);

	float mtxl=0, mtxr=0, mtxt=0, mtxb=0;
	if (use_alphashader) {
		alpha_texture->get_subtexture_coordinates(mtx, &mtxl, &mtxr, &mtxt, &mtxb);
	}

	//this array will be uploaded to the GPU.
	//it contains all dynamic vertex data (position, tex coordinates, mask coordinates)
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
	//glDrawArrays(GL_QUADS, 0, 4);

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
	
	
		Renderer::get().submit_quad(render_quad::Create(
		                            //Position to render to
		                            rect::Create(vertex2::Create(leftf, topf),
											     vertex2::Create(rightf, bottomf)),
		                            //UV coords for texture
		                            rect::Create(vertex2::Create(txl, txt),
												 vertex2::Create(txr, txb)),
		                            //UV coords for mask texture
		                            rect::Create(vertex2::Create(mtxl, mtxt),
												 vertex2::Create(mtxr, mtxb)),
								    0,        //z-value
		                            0),       //playerID
									this->id, //diffuse
									this->id, //Mask
									cType);
}


struct gamedata::subtexture *Texture::get_subtexture(int subid) {
	if (subid < (ssize_t)this->subtexture_count && subid >= 0) {
		return &this->subtextures[subid];
	}
	else {
		throw util::Error("requested unknown subtexture %d", subid);
	}
}


void Texture::get_subtexture_coordinates(int subid, float *txl, float *txr, float *txt, float *txb) {
	struct gamedata::subtexture *tx = this->get_subtexture(subid);
	this->get_subtexture_coordinates(tx, txl, txr, txt, txb);
}

void Texture::get_subtexture_coordinates(struct gamedata::subtexture *tx,
                                         float *txl, float *txr, float *txt, float *txb) {
	*txl = ((float)tx->x)           /this->w;
	*txr = ((float)(tx->x + tx->w)) /this->w;
	*txt = ((float)tx->y)           /this->h;
	*txb = ((float)(tx->y + tx->h)) /this->h;
}


int Texture::get_subtexture_count() {
	return this->subtexture_count;
}

void Texture::get_subtexture_size(int subid, int *w, int *h) {
	struct gamedata::subtexture *subtex = this->get_subtexture(subid);
	*w = subtex->w;
	*h = subtex->h;
}

GLuint Texture::get_texture_id() {
	return this->id;
}

} //namespace openage
