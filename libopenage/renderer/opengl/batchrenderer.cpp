#include "batchrenderer.h"
#include "../../log/log.h"
#include "../../error/error.h"



namespace openage{
namespace renderer{
namespace opengl{
    BatchRenderer::BatchRenderer(GlContext* context)
        :gl_context(context)
    {
    m_VBO = new opengl::GlBuffer(RENDERER_BUFFER_SIZE,GL_STATIC_DRAW);
    m_Vao = new opengl::GlVertexArray();
    m_Vao->bind();
    tex_mngr = new TextureManager;
    
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //add active ID 
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
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

    void BatchRenderer::submit(Sprite_2& sprite){
        
        tex_mngr->get_activeID(sprite);
        
        
        m_buffer->x = sprite.x;
        m_buffer->y = sprite.y;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.left;
        m_buffer->v = sprite.bottom;
        m_buffer->active_id = sprite.active_id;
        m_buffer++;

        m_buffer->x = sprite.x;
        m_buffer->y = sprite.y+ sprite.h;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.left;
        m_buffer->v = sprite.top;
        m_buffer->active_id = sprite.active_id;
        m_buffer++;

        m_buffer->x = sprite.x + sprite.w;
        m_buffer->y = sprite.y + sprite.h;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.right;
        m_buffer->v = sprite.top;
        m_buffer->active_id = sprite.active_id;
        m_buffer++;

        m_buffer->x = sprite.x + sprite.w;
        m_buffer->y = sprite.y;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.right;
        m_buffer->v = sprite.bottom;
        m_buffer->active_id = sprite.active_id;
        m_buffer++;

        m_indexcount += 6;
    }
    void BatchRenderer::begin(){
        m_VBO->bind(GL_ARRAY_BUFFER);
        m_buffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    }

    void BatchRenderer::end(){
        glUnmapBuffer(GL_ARRAY_BUFFER);
        m_VBO->unbind(GL_ARRAY_BUFFER);
    }

    void BatchRenderer::render(){
        
        tex_mngr->bind_textures();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Vao->bind();
        m_IBO->bind();

        glDrawElements(GL_TRIANGLES, m_indexcount, GL_UNSIGNED_INT, 0);

        m_IBO->unbind();
        m_Vao->unbind();

        m_indexcount = 0;


    }

    BatchRenderer::~BatchRenderer(){

    }

    std::shared_ptr<GlShaderProgram> BatchRenderer::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	    log::log(INFO << "max texture slots "<<this->gl_context->get_capabilities().max_texture_slots);
	    return std::make_shared<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
    }

    std::unique_ptr<Texture> BatchRenderer::add_texture(const resources::TextureData& data) {
	    return std::make_unique<GlTexture>(data);
    }
    
}
}
}