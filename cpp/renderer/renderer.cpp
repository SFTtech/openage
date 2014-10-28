#include "renderer.h"

#include "../shader/program.h"
#include "../shader/shader.h"

#include "../util/file.h"
#include "../util/dir.h"

#include <stdio.h>

namespace openage {
namespace graphics {
		
Renderer *Renderer::instance = NULL;
		
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
		{"shaders/alphamask.vert.glsl",  "shaders/alphamask.common.frag.glsl"},
		{NULL}
	};
	
	char const **cshader_file = shader_file_names[0];
	
	do {
		char *texture_vert_code;
		util::read_whole_file(&texture_vert_code, data_dir->join(cshader_file[0]));
		auto vert_shader = new shader::Shader(GL_VERTEX_SHADER, texture_vert_code);

		char *texture_frag_code;
		util::read_whole_file(&texture_frag_code, data_dir->join(cshader_file[1]));
		auto frag_shader = new shader::Shader(GL_FRAGMENT_SHADER, texture_frag_code);
		
		material &cMaterial = materials[eMaterialType::keNormal];
		
		cMaterial.program = new shader::Program(vert_shader, frag_shader);
		cMaterial.program->link();
		
		cMaterial.uniformNormalTexture = cMaterial.program->get_uniform_id("texture");
		cMaterial.uniformUV = cMaterial.program->get_uniform_id("tex_coordinates");
		
		cMaterial.uniformMasktexture = cMaterial.program->get_uniform_id("mask_texture");
		cMaterial.uniformMaskUV = cMaterial.program->get_uniform_id("mask_tex_coordinates");
		
		delete[] texture_vert_code;
		delete[] texture_frag_code;

		cshader_file += sizeof(shader_file_names[0]) / sizeof(shader_file_names[0][0]);
	} while ( cshader_file[0] != NULL );
	
	return true;
}
		
void Renderer::submit_quad(render_quad const & quad,
                           GLint diffuse,
                           GLint mask,
                           eMaterialType::Enum material_type) {
	//Stuff data into vertex buffer
	
	//Generate a renderer key from
	//	MaterialType(16bit) | {textureHandle(16bit) | maskHandle(16bit)} | QuadIdx (16bit)
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