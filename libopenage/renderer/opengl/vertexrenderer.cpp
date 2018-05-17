#include "vertexrenderer.h"
#include "../../log/log.h"
#include "../../error/error.h"



namespace openage{
namespace renderer{
namespace opengl{
    VertexRenderer::VertexRenderer(GlContext* context,util::Path& path)
        :gl_context(context),root(path)
    {
    m_VBO = new opengl::GlBuffer(RENDERER_BUFFER_SIZE,GL_DYNAMIC_DRAW);
    m_Vao = new opengl::GlVertexArray();
    m_Vao->bind();    
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
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

void VertexRenderer::begin(){
        m_VBO->bind(GL_ARRAY_BUFFER);
        m_buffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    }

    void VertexRenderer::end(){
        glUnmapBuffer(GL_ARRAY_BUFFER);
        m_VBO->unbind(GL_ARRAY_BUFFER);
    }

    void VertexRenderer::render(){
        m_Vao->bind();
        m_IBO->bind();

        glDrawElements(GL_TRIANGLES, m_indexcount, GL_UNSIGNED_INT, 0);

        m_IBO->unbind();
        m_Vao->unbind();

        m_indexcount = 0;


    }


    void VertexRenderer::submit(Sprite_2& sprite){
        
        m_buffer->x = sprite.x - sprite.w/2.0f;
        m_buffer->y = sprite.y - sprite.h/2.0f;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.left;
        m_buffer->v = sprite.bottom;
        m_buffer++;

        m_buffer->x = sprite.x - sprite.w/2.0f;
        m_buffer->y = sprite.y+ sprite.h - sprite.h/2.0f;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.left;
        m_buffer->v = sprite.top;
        m_buffer++;

        m_buffer->x = sprite.x + sprite.w - sprite.w/2.0f;
        m_buffer->y = sprite.y + sprite.h - sprite.h/2.0f;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.right;
        m_buffer->v = sprite.top;
        m_buffer++;

        m_buffer->x = sprite.x + sprite.w - sprite.w/2.0f;
        m_buffer->y = sprite.y - sprite.h/2.0f;
        m_buffer->r = sprite.r;
        m_buffer->g = sprite.g;
        m_buffer->b = sprite.b;
        m_buffer->a = sprite.a;
        m_buffer->u = sprite.right;
        m_buffer->v = sprite.bottom;
        m_buffer++;

        m_indexcount += 6;
    }

    std::shared_ptr<GlShaderProgram> VertexRenderer::add_shader(std::vector<resources::ShaderSource> const& srcs) {
	    log::log(INFO << "max texture slots "<<this->gl_context->get_capabilities().max_texture_slots);
	    return std::make_shared<GlShaderProgram>(srcs, this->gl_context->get_capabilities());
    }

    std::unique_ptr<Texture> VertexRenderer::add_texture(const resources::TextureData& data) {
	    return std::make_unique<GlTexture>(data);
    }

}}}