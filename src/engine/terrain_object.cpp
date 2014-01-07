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


/**
* destructor.
*
* remove all terrain_chunk->object pointers
* remove itself from the drawing list
*/
TerrainObject::~TerrainObject() {
	if(this->occupied_chunk_count == 0) {
		log::dbg("terrain_object: deleted myself");
		return;
	}

	//remove from drawing list
	for(unsigned i = 0; i < this->occupied_chunk[0]->object_list.size(); i++) {
		if(this->occupied_chunk[0]->object_list[i] == this)
			this->occupied_chunk[0]->object_list.erase(this->occupied_chunk[0]->object_list.begin()+i);
	}

	//remove terrain_chunk->object values
	//TODO: brute force, should be faster(?)
	for(int i = 0; i < this->occupied_chunk_count; i++) {
		for(unsigned pos = 0; pos < this->occupied_chunk[i]->get_tile_count(); pos++) {
			if(this->occupied_chunk[i]->object[pos] == this) {
				this->occupied_chunk[i]->object[pos] = nullptr;
			}
		}
	}
	log::dbg("terrain_object: deleted myself after removing some shit");
}

/**
* binds the TerrainObject to a certain TerrainChunk.
*
* main_chunk: pointer of chunk containing the upper left corner of the object
* pos: (tile) position of the upper left corner
*/
bool TerrainObject::bind_on_chunk(TerrainChunk *main_chunk, coord::tile pos) {
	//TODO: should work with more than one chunk
	//TODO: translate pos somehow, so that buildings get placed under not down-left of the cursor

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

	//TODO: to top_right: bigger is earlier, to bot_right: bigger is later
	main_chunk->object_list.push_back(this);

	log::dbg("terrain_object.bind: bound TerrainObject to chunk");
	return true;
}

bool TerrainObject::draw(){
	texture->draw(pos.to_tile3().to_phys3().to_camgame(), false, 0, player);

	return true;
}

} //namespace engine
