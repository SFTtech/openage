#include "renderer.h"

#include "../shader/program.h"
#include "../shader/shader.h"

#include "../util/file.h"
#include "../util/dir.h"

#include <stdio.h>

namespace openage {
namespace graphics {
		
Renderer *Renderer::instance = NULL;
	
//Local for now
GLuint vertbuf;

	
bool Renderer::create(util::Dir const *data_dir) {
	//Idempotent
	if ( instance != NULL ) return true;

	Renderer *nRenderer = new Renderer();
	
	if ( nRenderer->init(data_dir) ) {
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
		
bool Renderer::init(util::Dir const *data_dir) {
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
		
		//Needs to be an attribute
		cMaterial.attributeMaskUV = (cMaterial.program->has_attribute("mask_tex_coordinates")) ?
										cMaterial.program->get_attribute_id("mask_tex_coordinates")
										: -1;
		
		cMaterial.attributeUV = (cMaterial.program->has_attribute("tex_coordinates")) ?
										cMaterial.program->get_attribute_id("tex_coordinates")
										: -1;
		
		cMaterial.program->use();
		glUniform1i(cMaterial.uniformNormalTexture, 0);
		glUniform1i(cMaterial.uniformMasktexture, 1);
		cMaterial.program->stopusing();
		
		delete[] texture_vert_code;
		delete[] texture_frag_code;
	}
	
	glGenBuffers(1, &vertbuf);
	
	return true;
}
		
void Renderer::submit_quad(render_quad const & quad,
                           GLint diffuse,
                           GLint mask,
                           eMaterialType::Enum material_type) {
	//Stuff data into vertex buffer
	
	//Generate a renderer key from
	//	MaterialType(16bit) | {textureHandle(16bit) | maskHandle(16bit)} | QuadIdx (16bit)
	
	
	//For now we will render to quad immeadiatly (to make the transition easier)
	
	//if ( material_type != eMaterialType::keNormal ) return;
	if ( material_type == eMaterialType::keAlphaMask ) return;
	
	//
	//	Pre rendering per material type
	//
	
	switch (material_type) {
	case eMaterialType::keNormal: {
		//Render a normal texture here
		break;
	}
	default: {
		//Do nothing here
		break;
	}
	}
	
	material current_material = materials[material_type];
	
	current_material.program->use();
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	
	//Bind vertex buffers here
	glBindBuffer(GL_ARRAY_BUFFER, vertbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(render_quad), &quad.vertices, GL_STREAM_DRAW);
	

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
	
	//Set the masking layer here
	
	//draw the vertex array
	glDrawArrays(GL_QUADS, 0, 4);
	
	glDisableVertexAttribArray(current_material.program->pos_id);
	glDisableVertexAttribArray(current_material.attributeUV);

	//unbind the current buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	current_material.program->stopusing();
	
	//
	//	Post rendering per material type
	//
	switch (material_type) {
	case eMaterialType::keNormal: {
		//Render a normal texture here
		break;
	}
	default: {
		//Do nothing here
		break;
	}
	}
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

void Renderer::render() {
	//Sort the renderlist
	//Build the index buffer (dependent on sorting order of quads)
	//	Build a vector of *boundries* used when rendering
	
	
	//Bind vertex buffer
	//Bind index buffer
	
	
	//Clear the depth buffer
	//Enable depth test
	//Enable depth write
	
	
	//Foreach materialtype
	//	bind shader
	
	//	Foreach item in boundries
	//		drawelements using boundry information
}

}
}