#include "terrainmanager.h"


namespace openage{ 
namespace renderer{
namespace opengl{

    TerrainManager::TerrainManager(util::Path path){
        tex_array = new opengl::GlTextureArray(32,512,512,resources::pixel_format::rgba8);
        std::string url;
        for(int i = 1; i <= 32;i++){
            url = "/assets/terrain/textures/" + std::to_string(i) + ".png";
            textures.push_back(resources::TextureData(path/url,false));
            tex_array->submit_texture(textures.back());
        }

        auto vshader_src = resources::ShaderSource(
		    resources::shader_lang_t::glsl,
		    resources::shader_stage_t::vertex,
		    path / "/assets/test_shaders/batch.vert.glsl");

	    auto fshader_src = resources::ShaderSource(
		    resources::shader_lang_t::glsl,
		    resources::shader_stage_t::fragment,
		    path / "assets/test_shaders/batch.frag.glsl");
        auto shade = this->add_shader({ vshader_src, fshader_src });
        
    // vertex info setup
    m_VBO = new opengl::GlBuffer(RENDERER_BUFFER_SIZE,GL_DYNAMIC_DRAW);
    m_Vao = new opengl::GlVertexArray();
    m_Vao->bind();
    
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6* sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //add terrain index 
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // alpha mask bool
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(3);
    m_VBO->unbind(GL_ARRAY_BUFFER);

    GLuint indices_2[RENDERER_INDICES_SIZE];

        int offset = 0;
        for(int i = 0;i<RENDERER_INDICES_SIZE;i+=6){

            indices_2[i] = offset + 0;
            indices_2[i + 1] = offset + 1;
            indices_2[i + 2] = offset + 2;
            indices_2[i + 3] = offset + 2;
            indices_2[i + 4] = offset + 3;
            indices_2[i + 5] = offset + 0;

            offset += 4;
        }
    m_IBO = new opengl::GlIndexBuffer(indices_2,RENDERER_INDICES_SIZE*sizeof(GLuint),GL_STATIC_DRAW);
    m_Vao->unbind();

    }

    void TerrainManager::bind(){
        tex_array->bind();
    }




}}}