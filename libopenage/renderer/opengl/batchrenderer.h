#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "context.h"
#include "render_target.h"
#include "vertex_array.h"
#include "buffer.h"
#include "indexbuffer.h"
#include <eigen3/Eigen/Dense>
#include "shader_program.h"
#include "texture.h"
#include "texturemanager.h"

#define RENDERER_MAX_SPRITES 60000
#define RENDERER_VERTEX_SIZE 36
#define RENDERER_SPRITE_SIZE RENDERER_VERTEX_SIZE*4
#define RENDERER_BUFFER_SIZE RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE RENDERER_MAX_SPRITES*6

#define SHADER_VERTEX_INDEX 0
#define SHADER_COLOR_INDEX 1


namespace openage{
namespace renderer{
namespace opengl{

    struct VertexData{
       float x;
       float y;
       float r;
       float g;
       float b;
       float a;
       float u;
       float v;
       float active_id;
    };
        
    class BatchRenderer{
        
        public:

            BatchRenderer(GlContext*,util::Path& path);
            ~BatchRenderer();

            void begin();
            void submit(Sprite_2& sprite);
            void end();

            void render();
            std::shared_ptr<GlShaderProgram> add_shader(std::vector<resources::ShaderSource> const& srcs);
            std::unique_ptr<Texture> add_texture(const resources::TextureData& data);
            GlRenderTarget display;
            util::Path root;
        private:
	        /// The GL context.
	        GlContext *gl_context;
            GlBuffer* m_VBO;
            GlVertexArray* m_Vao;
            GlIndexBuffer* m_IBO;
            TextureManager* tex_mngr;
	    
            VertexData* m_buffer;
            int m_indexcount = 0;
    };



    
}
}
}