#include "terrain_object.h"

#include "engine.h"
#include "texture.h"
#include "log.h"
#include "terrain_chunk.h"
#include "util/error.h"
#include "coord/tile.h"
#include "coord/tile3.h"
#include "coord/phys3.h"
#include "coord/camgame.h"

namespace engine {


TerrainObject::TerrainObject(Texture *tex, unsigned player) {
	//university specific for testing purposes
	this->texture = tex;
	this->size = { 4, 4 };
	this->player = player;

	this->occupied_chunk_count = 0;

	log::dbg("terrain_object: created terrain_object");
}

TerrainObject::TerrainObject(unsigned player): TerrainObject(new Texture("age/raw/Data/graphics.drs/3836.slp.png", true, PLAYERCOLORED), player) {}


TerrainObject::~TerrainObject() {
	log::dbg("terrain_object: deleted myself");
}

/*
* binds the TerrainObject to a certain TerrainChunk.
*
* main_chunk: pointer of chunk containing the upper left corner of the object
* pos: (tile) position of the upper left corner
*/
bool TerrainObject::bind_on_chunk(TerrainChunk *main_chunk, coord::tile pos) {
	//TODO: should work with more than one chunk

	//check it it is even possible (only buildings TODO: check terrain tiles)
	coord::tile temp_pos = pos;
	for(unsigned i = 0; i < this->size.se_length; i++) {
		for(unsigned j = 0; j < this->size.ne_length; j++) {
			try {
				if(main_chunk->object[main_chunk->tile_position(temp_pos)] != nullptr) {
					log::dbg("terrain_object.bind: there is already another building");
					return false;
				}
				temp_pos.ne--;
			//i know, that u don't like that Jonas... but idc
			} catch(Error e){ //if some part of the building is outside of the map
				log::dbg("terrain_object.bind: no building in outer space");
				return false;
			}
		}
		temp_pos.ne = pos.ne;
		temp_pos.se++;
	}

	this->occupied_chunk_count = 1;
	this->occupied_chunk[0] = main_chunk;
	this->pos = pos;

	log::dbg("terrain_object.bind: trying to add pointers for object-pointers");
	temp_pos = this->pos;
	for(unsigned i = 0; i < this->size.se_length; i++) {
		for(unsigned j = 0; j < this->size.ne_length; j++) {
			main_chunk->object[main_chunk->tile_position(temp_pos)] = this;
			temp_pos.ne--;
		}
		temp_pos.ne = pos.ne;
		temp_pos.se++;
	}

	main_chunk->object_list.push_back(this);

	log::dbg("terrain_object.bind: bound TerrainObject to chunk");
	return true;
}

bool TerrainObject::draw(){
	texture->draw(pos.to_tile3().to_phys3().to_camgame(), false, 0, player);

	return true;
}

} //namespace engine
