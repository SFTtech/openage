#pragma once

#include "texture.h"
#include "../../util/path.h"
#include "sprite_2.h"
#include "../../log/log.h"
#include "../../error/error.h"
#include <vector>
namespace openage{
namespace renderer{
namespace opengl{
struct TextureStruct{
   resources::TextureData data;
    std::shared_ptr<Texture> tex;
};
class TextureManager{
    
    public:
        TextureManager(util::Path& path);
        int get_activeID(Sprite_2& sprite);
        std::unique_ptr<Texture> add_tex(resources::TextureData& data);
        TextureStruct add_texture(int tex_id,bool meta_file);
        int bind_textures();
        void getUV(Sprite_2& sprite);
    private:
        std::vector<TextureStruct> textures;
        bool is_change = true;
        util::Path root;
    
};

}}}