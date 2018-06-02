#include "terrainmanager.h"
#include "../../log/log.h"
#include "../../error/error.h"

namespace openage{ 
namespace renderer{
namespace opengl{

    TerrainManager::TerrainManager(GlContext* context,util::Path path)
    :gl_context(context){
        tex_array = new opengl::GlTextureArray(32,512,512,resources::pixel_format::rgba8);
        std::string url;
        for(int i = 44; i <= 76;i++){
            url = "/assets/terrain/textures/" + std::to_string(i) + ".png";
            textures.push_back(resources::TextureData(path/url,false));
            tex_array->submit_texture(textures.back());
        }

        auto vshader_src = resources::ShaderSource(
		    resources::shader_lang_t::glsl,
		    resources::shader_stage_t::vertex,
		    path / "/assets/test_shaders/terrain.vert.glsl");

	    auto fshader_src = resources::ShaderSource(
		    resources::shader_lang_t::glsl,
		    resources::shader_stage_t::fragment,
		    path / "assets/test_shaders/terrain.frag.glsl");
        shade = this->add_shader({ vshader_src, fshader_src });

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
    std::shared_ptr<GlShaderProgram> TerrainManager::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	    log::log(INFO << "max texture slots "<<this->gl_context->get_capabilities().max_texture_slots);
	    return std::make_shared<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
    }

    void TerrainManager::init_terrain(){
        for(int i = 0;i<35;i++){
            for(int j = 0;j<35;j++){
                terrain_layout[i][j] = rand()%32;
            }
        }
    }

    void TerrainManager::submit(){
        
        float offset_x = 0;
        float offset_y = -1100;
        for(int i = 0;i<35;i++){
            for(int j = 0;j<35;j++){
                float left = 0.125*(rand()%8);
                float top = 0.125*(rand()%8);
                float temp_tex = rand()%32;                
                m_buffer->x = offset_x + 64*j;
                m_buffer->y = offset_y + 64*i;
                m_buffer->u = left;
                m_buffer->v =  top + 0.125;
                m_buffer->is_alpha = 0;
                m_buffer->tex_index = terrain_layout[i][j];
                m_buffer++;
                m_buffer->x = offset_x + 64*j;
                m_buffer->y = offset_y + 64*i + 64;
                m_buffer->u = left;
                m_buffer->v = top;
                m_buffer->is_alpha = 0;
                m_buffer->tex_index = terrain_layout[i][j];
                m_buffer++;
                m_buffer->x = offset_x + 64*j + 64;
                m_buffer->y = offset_y + 64*i + 64;
                m_buffer->u = left + 0.125;
                m_buffer->v = top;
                m_buffer->is_alpha = 0;
                m_buffer->tex_index = terrain_layout[i][j];
                m_buffer++;
                m_buffer->x = offset_x + 64*j + 64;
                m_buffer->y = offset_y + 64*i;
                m_buffer->u = left + 0.125;
                m_buffer->v = top + 0.125;
                m_buffer->is_alpha = 0;
                m_buffer->tex_index = terrain_layout[i][j];
                m_buffer++;
                m_indexcount += 6;
            }
        }
    }
    void TerrainManager::begin(){
        m_VBO->bind(GL_ARRAY_BUFFER);
        m_buffer = (TerrainData*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    }

    void TerrainManager::end(){
        glUnmapBuffer(GL_ARRAY_BUFFER);
        m_VBO->unbind(GL_ARRAY_BUFFER);
    }

    void TerrainManager::render(){
        Eigen::Matrix4f pers2;
    pers2 << 	2.0f/(1920.0f),0,0,-1.0f,
				0,-2.0f/(1080.0f),0,1.0f,
				0,  0,1.0f,0,
				0.0f,0.0f,  1.0f,  1;
    Eigen::Matrix4f dimet; /// This is the DIMETRIC Perspective. Used in age of empires (Reference: https://gamedev.stackexchange.com/questions/16746/what-is-the-name-of-perspective-of-age-of-empires-ii)
	    dimet << 	1.0,1.0,0,0,
	            	0.5,-0.5,1.0,0,
	            	0,0,0,0,
	            	0,0,0,1;
        shade->use();
        shade->sampler_array(0); 
        auto batch_uniform = shade->new_uniform_input("ortho",pers2,"dimet",dimet);
        auto lala_batch = dynamic_cast<opengl::GlUniformInput const*>(batch_uniform.get());
        shade->execute_with(lala_batch,nullptr);      
        this->init_terrain();
        this->begin();
        this->submit();
        this->end();
        m_Vao->bind();
        //m_IBO->bind();

        glDrawElements(GL_TRIANGLES, m_indexcount, GL_UNSIGNED_INT, 0);

        //m_IBO->unbind();
        m_Vao->unbind();
        m_indexcount = 0;
    }

}}}