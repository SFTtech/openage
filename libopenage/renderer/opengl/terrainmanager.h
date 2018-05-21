#include <vector>
#include "texturearray.h"
#include "../../util/path.h"
#include "shader_program.h"
#include "vertex_array.h"
#include "buffer.h"
#include "indexbuffer.h"

#define RENDERER_MAX_SPRITES 100000
#define RENDERER_VERTEX_SIZE 40
#define RENDERER_SPRITE_SIZE RENDERER_VERTEX_SIZE*4
#define RENDERER_BUFFER_SIZE RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE RENDERER_MAX_SPRITES*6

namespace openage{ 
namespace renderer{
namespace opengl{
struct TerrainData{
       float x;
       float y;
       float u;
       float v;
       float tex_index;
       float is_alpha;
    };
class TerrainManager{
    public:
        TerrainManager(util::Path path);
        std::shared_ptr<GlShaderProgram> add_shader(std::vector<resources::ShaderSource> const& srcs);
        void bind();
    private:
        std::vector<resources::TextureData> textures;
        GlTextureArray* tex_array;
};




}}}