#include "renderer.h"

#include "../shader/program.h"
#include "../shader/shader.h"

#include "../util/file.h"
#include "../util/dir.h"
#include "../gamedata/color.gen.h"

#include <algorithm>
#include <stdio.h>

namespace openage {
namespace graphics {
		
Renderer *Renderer::instance = NULL;
	
//Local for now
GLuint vertbuf;

	
bool Renderer::create(util::Dir const *data_dir, util::Dir const *asset_dir) {
	//Idempotent
	if ( instance != NULL ) return true;

	Renderer *nRenderer = new Renderer();
	
	if ( nRenderer->init(data_dir, asset_dir) ) {
		instance = nRenderer;
		return true;
	}
	
	delete nRenderer;
	return false;
}
		
Renderer &Renderer::get() {
	//TODO: Should assert here if instance == NULL
	
	return *instance;
}
		
		
Renderer::Renderer() {
	//Constructor 
}
		
bool Renderer::init(util::Dir const *data_dir, util::Dir const *asset_dir) {
	const char *shader_file_names[][2] = {
		{"shaders/maptexture.vert.glsl", "shaders/maptexture.frag.glsl"},
		{"shaders/maptexture.vert.glsl", "shaders/teamcolors.frag.glsl"},
		{"shaders/alphamask.common.vert.glsl", "shaders/alphamask.common.frag.glsl"},
		{NULL}
	};
	
	
	
	for ( int cmaterial_idx = 0; cmaterial_idx < eMaterialType::keCount; cmaterial_idx++)
	{
		char const **cshader_file = shader_file_names[cmaterial_idx];
		
		printf ("Loading shaders: %s %s\n", cshader_file[0], cshader_file[1]);
		
		char *texture_vert_code;
		util::read_whole_file(&texture_vert_code, data_dir->join(cshader_file[0]));
		auto vert_shader = new shader::Shader(GL_VERTEX_SHADER, texture_vert_code);

		char *texture_frag_code;
		util::read_whole_file(&texture_frag_code, data_dir->join(cshader_file[1]));
		auto frag_shader = new shader::Shader(GL_FRAGMENT_SHADER, texture_frag_code);
		
		material &cMaterial = materials[cmaterial_idx];
		
		cMaterial.program = new shader::Program(vert_shader, frag_shader);
		cMaterial.program->link();
		
		cMaterial.uniformNormalTexture = cMaterial.program->get_uniform_id("texture");
		cMaterial.uniformMasktexture = cMaterial.program->get_uniform_id("mask_texture");
		cMaterial.uniformPlayerColors = cMaterial.program->get_uniform_id("player_color");
		
		//Needs to be an attribute
		cMaterial.attributeMaskUV = (cMaterial.program->has_attribute("mask_tex_coordinates")) ?
										cMaterial.program->get_attribute_id("mask_tex_coordinates")
										: -1;
		
		cMaterial.attributeUV = (cMaterial.program->has_attribute("tex_coordinates")) ?
										cMaterial.program->get_attribute_id("tex_coordinates")
										: -1;

		cMaterial.attributePlayerCol = (cMaterial.program->has_attribute("player_number")) ?
										cMaterial.program->get_attribute_id("player_number")
										: -1;
		
		cMaterial.attributeZOrder = (cMaterial.program->has_attribute("z_order")) ?
										cMaterial.program->get_attribute_id("z_order")
										: -1;
		
		//Fixup material specific
		
		cMaterial.program->use();
		glUniform1i(cMaterial.uniformNormalTexture, 0);
		glUniform1i(cMaterial.uniformMasktexture, 1);
		cMaterial.program->stopusing();
		
		delete[] texture_vert_code;
		delete[] texture_frag_code;
	}
	
	{
		//fixup material specific normals
		auto player_color_lines = util::read_csv_file<gamedata::palette_color>(asset_dir->join("player_palette_50500.docx"));
		
		GLfloat *playercolors = new GLfloat[player_color_lines.size() * 4];
		for (size_t i = 0; i < player_color_lines.size(); i++) {
			auto line = &player_color_lines[i];
			playercolors[i*4]     = line->r / 255.0;
			playercolors[i*4 + 1] = line->g / 255.0;
			playercolors[i*4 + 2] = line->b / 255.0;
			playercolors[i*4 + 3] = line->a / 255.0;
		}
		
		
		//
		//	TODO: Update these to a 1D texture
		//
		materials[eMaterialType::keColorReplace].program->use();
		glUniform4fv(materials[eMaterialType::keColorReplace].uniformPlayerColors, 64, playercolors);
		materials[eMaterialType::keColorReplace].program->stopusing();
	}
	glGenBuffers(1, &vertbuf);
	
	return true;
}
		
void Renderer::submit_quad(render_quad const & quad,
                           GLint diffuse,
                           GLint mask,
						   unsigned char layer,
                           eMaterialType::Enum material_type) {
	
	//Generate a renderer key from
	//	MaterialType(16bit) | {textureHandle(16bit) | maskHandle(16bit)} | QuadIdx (16bit)
	
	int cIndex = render_queue.size();
	
	render_token_struct pData = {
			(short)cIndex,
			(short)mask,
			(short)diffuse,
			(unsigned char)material_type,
			layer,
	};
	
	//Pack this into a 64bit unsigned long
	render_token nToken = *((render_token*)&pData);
	render_queue.push_back(nToken);
	render_buffer.push_back(quad);
	
	//Render immediately for testing purposes
	//render();
	
}
	
void Renderer::apply_material (eMaterialType::Enum material_type) {
	material current_material = materials[material_type];
	
	//if (material_type == eMaterialType::keNormal) continue;
	
	current_material.program->use();
	
	//	Need to retrieve the attributes from the shaders before we can use them here
	glEnableVertexAttribArray(current_material.program->pos_id);
	glEnableVertexAttribArray(current_material.attributeUV);
	
	//set data types, offsets in the vdata array
	glVertexAttribPointer(current_material.program->pos_id,
						  2,
						  GL_FLOAT,
						  GL_FALSE,
						  sizeof(render_quad::quad_vertex),
						  (void*)offsetof(render_quad::quad_vertex, pos));
	
	glVertexAttribPointer(current_material.attributeUV,
						  2,
						  GL_FLOAT,
						  GL_FALSE,
						  sizeof(render_quad::quad_vertex),
						  (void*)offsetof(render_quad::quad_vertex, uv));
	
	if ( material_type == eMaterialType::keAlphaMask ) {
		glEnableVertexAttribArray(current_material.attributeMaskUV);
		glVertexAttribPointer(current_material.attributeMaskUV,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(render_quad::quad_vertex),
							  (void*)offsetof(render_quad::quad_vertex, maskUV));
		
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
	}
	
	//These checks are temporary
	if ( current_material.attributePlayerCol != -1 )
	{
		glEnableVertexAttribArray(current_material.attributePlayerCol);
		glVertexAttribPointer(current_material.attributePlayerCol,
							  1,
							  GL_UNSIGNED_INT,
							  GL_FALSE,
							  sizeof(render_quad::quad_vertex),
							  (void*)offsetof(render_quad::quad_vertex, playerID));
	}
	if ( current_material.attributeZOrder != -1 )
	{
		glEnableVertexAttribArray(current_material.attributeZOrder);
		glVertexAttribPointer(current_material.attributeZOrder,
							  1,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(render_quad::quad_vertex),
							  (void*)offsetof(render_quad::quad_vertex, zValue));
	}
}

void Renderer::disable_material (eMaterialType::Enum material_type){
	material current_material = materials[material_type];
	
	glDisableVertexAttribArray(current_material.program->pos_id);
	glDisableVertexAttribArray(current_material.attributeUV);
	
	//These checks are temporary
	if ( current_material.attributePlayerCol != -1 )
	{
		glDisableVertexAttribArray(current_material.attributePlayerCol);
	}
	if ( current_material.attributeZOrder != -1 )
	{
		glDisableVertexAttribArray(current_material.attributeZOrder);
	}
	
	if ( material_type == eMaterialType::keAlphaMask) {
		glDisableVertexAttribArray(current_material.attributeMaskUV);
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
	}
	
	current_material.program->stopusing();
}
	
void renderBuffer (std::vector<unsigned short> &idxBuffer,
				   eMaterialType::Enum material_type,
				   GLint diffuse,
				   GLint mask)
{
	//Render out
	if ( material_type == eMaterialType::keAlphaMask )
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mask);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	
	//draw the vertex array
	glDrawElements(GL_QUADS, idxBuffer.size(), GL_UNSIGNED_SHORT, &idxBuffer[0]);
	//glDrawArrays(GL_QUADS, renderCommand.idx * 4, 4);
	
	idxBuffer.clear();
}
	
void Renderer::render() {
	int quadCount = render_queue.size();
	int bufferSize = sizeof(render_quad) * quadCount;
	
	glClearDepth(0);
	glDepthMask(true);
	

	glEnable (GL_DEPTH_TEST);
	glDepthFunc(GL_GEQUAL);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertbuf);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, &render_buffer[0].vertices, GL_STREAM_DRAW);
	
	//Sort the draw calls
	std::sort(render_queue.begin(), render_queue.end());
	
	render_token_struct lastCommand = *((render_token_struct*)&render_queue[render_queue.size()-1]);
	unsigned char maxLayer = lastCommand.layer;
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	std::vector<unsigned short> indexBuffer;
	
	int cQuad = 0;
	for ( int cLayer = 0; cLayer <= maxLayer; cLayer++)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		for ( int cMaterial = 0; cMaterial < eMaterialType::keCount; cMaterial++)
		{
			eMaterialType::Enum material_type = (eMaterialType::Enum)cMaterial;
			apply_material(material_type);
			
			//Render all the quads for this material
			while ( true )
			{
				render_token_struct renderCommand = *((render_token_struct*)&render_queue[cQuad]);
				GLint textureCombo = *((GLint*)&renderCommand.diffuse);
				
				if ( (eMaterialType::Enum)renderCommand.matType != material_type )
				{
					//printf ("Batching %lu on material change\n", indexBuffer.size());
					renderBuffer(indexBuffer, material_type, renderCommand.diffuse, renderCommand.mask);
					break;
				}
				
				indexBuffer.push_back((renderCommand.idx * 4));
				indexBuffer.push_back((renderCommand.idx * 4) +1);
				indexBuffer.push_back((renderCommand.idx * 4) +2);
				indexBuffer.push_back((renderCommand.idx * 4) +3);
				

				cQuad++;
				if ( cQuad >= quadCount)
				{
					//printf ("Completing render %lu\n", indexBuffer.size());
					renderBuffer(indexBuffer, material_type, renderCommand.diffuse, renderCommand.mask);
					break;
				}
				
				render_token_struct nextRenderCommand = *((render_token_struct*)&render_queue[cQuad]);
				GLint nextTextureCombo = *((GLint*)&nextRenderCommand.diffuse);
				
				//Check for texture change
				if ((eMaterialType::Enum)nextRenderCommand.matType != material_type
					|| nextTextureCombo != textureCombo)
				{
					//printf ("Batching %lu on texture change\n", indexBuffer.size());
					renderBuffer(indexBuffer, material_type, renderCommand.diffuse, renderCommand.mask);
					//renderBuffer(indexBuffer, material_type, renderCommand.diffuse, renderCommand.diffuse);
				}
			}
			
			//Render
			
			disable_material(material_type);
		}

	}
	
	//unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
	glDisable (GL_DEPTH_TEST);
	glDepthMask(false);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	render_queue.clear();
	render_buffer.clear();
}

}
}