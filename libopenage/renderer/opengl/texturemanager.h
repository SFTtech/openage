#pragma once

#include "texture.h"
#include "../../util/path.h"
#include "sprite_2.h"
#include <vector>
namespace openage{
namespace renderer{
namespace opengl{

class TextureManager{
    
    public:
        int get_activeID(Sprite_2& sprite);
        int add_texture(int tex_id);
        std::unique_ptr<Texture> add_texture(const resources::TextureData& data);
        int bind_textures();    
    private:
        std::vector<std::shared_ptr<Texture>> textures;
        bool is_change = true;

    
};

}}}