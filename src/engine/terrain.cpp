#include "terrain.h"

#include "engine.h"
#include "texture.h"
#include "log.h"
#include "util/error.h"
#include "util/strings.h"
#include "util/misc.h"
#include "coord/tile.h"
#include "coord/tile3.h"
#include "coord/phys3.h"
#include "coord/camgame.h"

namespace engine {

//TODO: get that from the convert script!
coord::camgame_delta tile_halfsize = {48, 24};


Terrain::Terrain(unsigned int size, size_t terrain_meta_count, TerrainType *terrain_meta, size_t blending_meta_count, BlendingMode *blending_meta) {
	this->size       = size;
	this->num_rows   = 2*size - 1;
	this->tile_count = size * size;

	//the ids of terraintype pieces on the terrain
	this->tiles = new int[this->tile_count];

	this->terrain_type_count = terrain_meta_count;
	this->blendmode_count    = blending_meta_count;
	this->textures       = new engine::Texture*[this->terrain_type_count];
	this->blending_masks = new engine::Texture*[this->blendmode_count];
	this->terrain_id_priority_map = new int[terrain_type_count];
	this->terrain_id_blendmode_map = new int[terrain_type_count];
	this->blending_enabled = true;

	log::dbg("terrain prefs: %lu tiletypes, %lu blendmodes", this->terrain_type_count, this->blendmode_count);

	for (size_t i = 0; i < this->terrain_type_count; i++) {
		auto line = &terrain_meta[i];
		this->terrain_id_priority_map[i] = line->blend_priority;
		this->terrain_id_blendmode_map[i] = line->blend_mode;

		char *terraintex_filename = util::format("age/raw/Data/terrain.drs/%d.slp.png", line->slp_id);
		auto new_texture = new Texture(terraintex_filename, true, ALPHAMASKED);
		new_texture->fix_hotspots(48, 24);
		this->textures[i] = new_texture;
		delete[] terraintex_filename;
	}

	for (size_t i = 0; i < this->blendmode_count; i++) {
		auto line = &blending_meta[i];

		char *mask_filename = util::format("age/alphamask/mode%02d.png", line->mode_id);
		auto new_texture = new Texture(mask_filename, true);
		new_texture->fix_hotspots(48, 24);
		this->blending_masks[i] = new_texture;
		delete[] mask_filename;
	}


	//set all tiles on the terrain id to 0 by default.
	for (unsigned int i = 0; i < this->tile_count; i++) {
		this->tiles[i] = 0;
	}

	log::dbg("created terrain: %lu size, %lu rows, %lu tiles", this->size, this->num_rows, this->tile_count);
}

Terrain::~Terrain() {
	for (size_t i = 0; i < this->terrain_type_count; i++) {
		delete this->textures[i];
	}
	for (size_t i = 0; i < this->blendmode_count; i++) {
		delete this->blending_masks[i];
	}

	delete[] this->blending_masks;
	delete[] this->tiles;
	delete[] this->textures;
	delete[] this->terrain_id_priority_map;
}

coord::tile_delta const neigh_offsets[] = {
	{ 1, -1},
	{ 1,  0},
	{ 1,  1},
	{ 0,  1},
	{-1,  1},
	{-1,  0},
	{-1, -1},
	{ 0, -1}
};

void Terrain::draw() {
	const bool respect_diagonal_influence = true;
	const bool respect_adjacent_influence = true;

	if (!respect_adjacent_influence && !respect_diagonal_influence) {
		this->blending_enabled = false;
	}

	coord::tile tilepos = {0, 0};
	for (; tilepos.ne < (int) this->size; tilepos.ne++) {
		for (tilepos.se = 0; tilepos.se < (int) this->size; tilepos.se++) {
			size_t terrain_id = this->get_tile(tilepos);

			if (terrain_id >= this->terrain_type_count) {
				throw Error("unknown terrain id=%lu requested for drawing", terrain_id);
			}
			int base_priority = this->terrain_id_priority_map[terrain_id];

			auto base_texture = this->textures[terrain_id];
			int sub_id = this->get_subtexture_id(tilepos, base_texture->atlas_dimensions);

			//draw the base texture
			base_texture->draw(tilepos.to_tile3().to_phys3().to_camgame(), false, sub_id);

			if (!this->blending_enabled) {
				continue;
			}

			//blendomatic!!111
			// see doc/media/blendomatic for the idea behind this.

			//storage for influences by neighbor tile
			struct influence_meta {
				int direction;
				int priority;
				int terrain_id;
			};

			int influence_tids[8];
			int num_influences = 0;

			auto influences = new struct influence_meta[this->terrain_type_count];
			for (unsigned int k = 0; k < this->terrain_type_count; k++) {
				influences[k].direction = 0;
				influences[k].priority = -1;
			}

			/* neighbor ids:
			      0
			    7   1
			  6   @   2
			    5   3
			      4
			*/


			//first step: gather information about possible influences
			//look at every neighbor tile for that
			for (int neigh_id = 0; neigh_id < 8; neigh_id++) {
				if (!respect_diagonal_influence && (neigh_id % 2) == 0) {
					continue;
				}

				if (!respect_adjacent_influence && (neigh_id % 2) == 1) {
					continue;
				}

				//calculate the pos of the neighbor tile
				coord::tile neigh_pos = tilepos + neigh_offsets[neigh_id];

				if (neigh_pos.ne < 0 || neigh_pos.ne >= (int)this->size || neigh_pos.se < 0 || neigh_pos.se >= (int)this->size) {
					//this neighbor is on the neighbor chunk or not existing, skip it for now.
					continue;
				}

				size_t neighbor_terrain_id = this->get_tile(neigh_pos);
				int neighbor_priority = this->terrain_id_priority_map[neighbor_terrain_id];

				//neighbor only interesting if it's a different terrain than @
				//if it is the same priority, the neigh priority is not higher.
				if (neighbor_terrain_id != terrain_id) {

					//neighbor draws over the base if it's priority is greater
					if (neighbor_priority > base_priority) {
						auto influence = &influences[neighbor_terrain_id];

						//this terrain id has no influence yet:
						if (influence->priority == -1) {
							influence_tids[num_influences] = neighbor_terrain_id;
							num_influences += 1;
						}

						//as tile i has influence for this priority
						// => bit i is set to 1 by 2^i
						influence->direction |= 1 << neigh_id;
						influence->priority = neighbor_priority;
						influence->terrain_id = neighbor_terrain_id;
					}
				}
			}

			//no blending necessary for this tile, it has no external influences.
			if (num_influences <= 0) {
				delete[] influences;
				continue;
			}

			//influences to consider when drawing overlays
			struct influence_meta draw_influences[8];

			//remove the influences terrain_id grouping
			for (int k = 0; k < num_influences; k++) {
				draw_influences[k] = influences[ influence_tids[k] ];
			}
			delete[] influences;

			//order the influences by their priority
			for (int k = 0; k < num_influences; k++) {
				struct influence_meta tmp_influence = draw_influences[k];

				int l = k - 1;
				while (l >= 0 && draw_influences[l].priority > tmp_influence.priority) {
					draw_influences[l + 1] = draw_influences[l];
					l -= 1;
				}

				draw_influences[l + 1] = tmp_influence;
			}

			struct draw_mask {
				int mask_id;
				int blend_mode;
				int terrain_id;
			};

			int mask_count = 0;
			struct draw_mask draw_masks[4+4]; //maximum 4 diagonal and 4 adjacent masks

			//for each possible influence (max 8 as we have 8 neighbors)
			for (int k = 0; k < num_influences; k++) {
				unsigned int binf = draw_influences[k].direction & 0xFF; //0b11111111
				if (binf == 0) {
					continue;
				}

				int neighbor_terrain_id = draw_influences[k].terrain_id;
				int adjacent_mask_id = -1;

				//log::dbg2("priority %d => mask %d, terrain %d", k, binf, neighbor_terrain_id);

				/* neighbor ids:
				      0
				    7   1      => 8 neighbors that can have influence on
				  6   @   2         the mask id selection.
				    5   3
				      4
				*/

				//ignore diagonal influences for adjacent influences
				int binfadjacent = binf & 0xAA; //0b10101010
				int binfdiagonal = binf & 0x55; //0b01010101

				//comment by mic_e TODO replace this by a lookup table
				if (respect_adjacent_influence) {
					switch (binfadjacent) {
					case 0x08:  //0b00001000
						adjacent_mask_id = 0;  //0..3
						break;
					case 0x02:  //0b00000010
						adjacent_mask_id = 4;  //4..7
						break;
					case 0x20:  //0b00100000
						adjacent_mask_id = 8;  //8..11
						break;
					case 0x80:  //0b10000000
						adjacent_mask_id = 12; //12..15
						break;
					case 0x22:  //0b00100010
						adjacent_mask_id = 20;
						break;
					case 0x88:  //0b10001000
						adjacent_mask_id = 21;
						break;
					case 0xA0:  //0b10100000
						adjacent_mask_id = 22;
						break;
					case 0x82:  //0b10000010
						adjacent_mask_id = 23;
						break;
					case 0x28:  //0b00101000
						adjacent_mask_id = 24;
						break;
					case 0x0A:  //0b00001010
						adjacent_mask_id = 25;
						break;
					case 0x2A:  //0b00101010
						adjacent_mask_id = 26;
						break;
					case 0xA8:  //0b10101000
						adjacent_mask_id = 27;
						break;
					case 0xA2:  //0b10100010
						adjacent_mask_id = 28;
						break;
					case 0x8A:  //0b10001010
						adjacent_mask_id = 29;
						break;
					case 0xAA:  //0b10101010
						adjacent_mask_id = 30;
						break;
					}
				}

				//if it's the plain adjacent mask, use all of the 4 possible masks.
				if (adjacent_mask_id <= 12 && adjacent_mask_id % 4 == 0) {
					int anti_redundancy_offset = util::mod<coord::tile_t>(tilepos.ne + tilepos.se, 4);
					adjacent_mask_id += anti_redundancy_offset;
				}

				//get the blending mode (the mask selection) for this transition
				int blend_mode = this->get_blending_mode(terrain_id, neighbor_terrain_id);

				if (adjacent_mask_id < 0) {
					if (respect_adjacent_influence && !respect_diagonal_influence && binfdiagonal == 0) {
						throw Error("influence detected with unknown directions: %u = 0x%02X", binf, binf);
					}
				} else if (respect_adjacent_influence) {
					draw_masks[mask_count].mask_id    = adjacent_mask_id;
					draw_masks[mask_count].blend_mode = blend_mode;
					draw_masks[mask_count].terrain_id = neighbor_terrain_id;
					mask_count += 1;
				}

				if (respect_diagonal_influence) {
					for (int l = 0; l < 4; l++) {
						//generate the neighbor id bit.
						int bdiaginf = 1 << (l*2);
						const int diag_mask_id_map[4] = {18, 16, 17, 19};

						// l == 0: pos = 0b000000001, mask = 18
						// l == 1: pos = 0b000000100, mask = 16
						// l == 2: pos = 0b000010000, mask = 17
						// l == 3: pos = 0b001000000, mask = 19
						if (binf & bdiaginf) {
							draw_masks[mask_count].mask_id    = diag_mask_id_map[l];
							draw_masks[mask_count].blend_mode = blend_mode;
							draw_masks[mask_count].terrain_id = neighbor_terrain_id;
							mask_count += 1;
						}
					}
				}
			}

			for (int k = 0; k < mask_count; k++) {
				//mask, to be applied on neighbor_terrain_id tile
				auto draw_mask = &draw_masks[k];
				auto overlay_texture = this->textures[draw_mask->terrain_id];
				int neighbor_sub_id = this->get_subtexture_id(tilepos, overlay_texture->atlas_dimensions);

				overlay_texture->activate_alphamask(this->blending_masks[draw_mask->blend_mode], draw_mask->mask_id);
				overlay_texture->draw(tilepos.to_tile3().to_phys3().to_camgame(), false, neighbor_sub_id);
				overlay_texture->disable_alphamask();
			}
		}
	}
}


/**
returns the terrain subtexture id for a given position.

this function returns always the right value, so that neighbor tiles
of the same terrain (like grass-grass) are matching (without blendomatic).
*/
unsigned Terrain::get_subtexture_id(coord::tile pos, unsigned atlas_size) {
	unsigned result = 0;

	result += util::mod<coord::tile_t>(pos.se, atlas_size);
	result *= atlas_size;
	result += util::mod<coord::tile_t>(pos.ne, atlas_size);

	return result;
}

void Terrain::set_tile(coord::tile pos, int tile) {
	tiles[tile_position(pos)] = tile;
}

int Terrain::get_tile(coord::tile pos) {
	return tiles[tile_position(pos)];
}

/**
calculates the memory position of a given tile location.

give this function isometric coordinates, it returns the tile index.

# is a single terrain tile:

         3
       2   #
     1   #   #
x= 0   #   *   #
     #   #   #   #
y= 0   #   #   #
     1   #   #
       2   #
         3

for example, * is at position (2, 1)
the returned index would be 6 (count for each x row, starting at y=0)
*/
size_t Terrain::tile_position(coord::tile pos) {
	if (pos.ne >= (int) this->size || pos.ne < 0 || pos.se >= (int) this->size || pos.se < 0) {
		throw Error("requested tile (%ld, %ld) that's not on this terrain.", pos.ne, pos.se);
	}

	return pos.se * this->size + pos.ne;
}

/**
calculates the memory position of a given diagonal tile location.

this does not respect the isometric coordinates, it's for drawn rows.

@OBSOLETE FOR NOW
*/
size_t Terrain::tile_position_diag(unsigned int row, unsigned int offset) {
	int so_far; //number of tiles in memory before the row
	unsigned int in_row; //number of tiles in the destination row

	if (row > this->num_rows - 1) {
		throw Error("Requested row %u, but there are only %lu rows in terrain.", row, this->num_rows);
	}

	//calculation if selected tile is in the upper half
	if (row <= this->num_rows/2) {
		so_far = (row*row + row)/2;
		in_row = row + 1;
	}
	//else the selected tile is in the lower half
	else {
		int brow = (this->num_rows - 1) - row + 1;
		so_far = this->size*this->size - ((brow*brow + brow)/2);
		in_row = brow;
	}

	if (offset > in_row-1) {
		throw Error("Requested tile %d of row %d which has only %d tiles", offset, row, in_row);
	}

	size_t position = so_far + offset;

	return position;
}

size_t Terrain::get_tile_count() {
	return this->tile_count;
}

void Terrain::set_texture(size_t index, engine::Texture *t) {
	this->textures[index] = t;
}

engine::Texture *Terrain::get_texture(size_t index) {
	return this->textures[index];
}

size_t Terrain::tiles_in_row(unsigned int row) {
	unsigned int in_row; //number of tiles in the destination row

	if (row > this->num_rows - 1) {
		throw Error("Requested row %u, but there are only %lu rows in terrain.", row, this->num_rows);
	}

	if (row <= this->num_rows/2) {
		in_row = row + 1;
	}
	else {
		in_row = (this->num_rows - 1) - row + 1;
	}
	return in_row;
}

size_t Terrain::get_size() {
	return this->size;
}

void Terrain::set_mask(unsigned int modeid, engine::Texture *m) {
	this->blending_masks[modeid] = m;
}

/**
return the blending mode id for two given neighbor ids.
*/
int Terrain::get_blending_mode(size_t base_id, size_t neighbor_id) {
	if (neighbor_id < base_id) {
		return this->terrain_id_blendmode_map[base_id];
	} else {
		return this->terrain_id_blendmode_map[neighbor_id];
	}
}



/**
parse and store a given line of a texture meta file.

this is used for reading all the lines of a .docx meta file
generated by the convert script.
*/
int TerrainType::fill(const char *by_line) {
	log::msg("filling line by %s", by_line);
	if (sscanf(by_line, "%u=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
	           &this->idx,
	           &this->terrain_id,
	           &this->slp_id,
	           &this->sound_id,
	           &this->blend_mode,
	           &this->blend_priority,
	           &this->angle_count,
	           &this->frame_count,
	           &this->terrain_dimensions0,
	           &this->terrain_dimensions1,
	           &this->terrain_replacement_id
	           )) {
		return 0;
	}
	else {
		return -1;
	}
}

/**
parse and store a blending mode description line.
*/
int BlendingMode::fill(const char *by_line) {
	log::msg("filling line by %s", by_line);
	if (sscanf(by_line, "%u=%d",
	           &this->idx,
	           &this->mode_id
	           )) {
		return 0;
	}
	else {
		return -1;
	}
}

} //namespace engine
