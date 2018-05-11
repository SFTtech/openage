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
    std::unique_ptr<Texture> tex;
};
class TextureManager{
    
    public:
        int get_activeID(Sprite_2& sprite);
        std::unique_ptr<Texture> add_texture(resources::TextureData* data);
        int bind_textures();    
    private:
        std::vector<std::shared_ptr<Texture>> textures;
        bool is_change = true;

    
};

}}}