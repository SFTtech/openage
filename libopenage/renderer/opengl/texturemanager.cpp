#include "texturemanager.h"


namespace openage{ 
namespace renderer{
namespace opengl{

int TextureManager::get_activeID(Sprite_2& sprite){
    
    if(sprite.is_tex == false){
        sprite.active_id = -1;
        return -1;
    }
    else if(sprite.is_tex == true && sprite.active_id == -1){
        is_change == true;
        auto texture = this->add_texture(sprite.texture_data);
        texture->tex_id = sprite.tex_id;

        if(textures.size() < 32){
            textures.push_back(std::move(texture));
            sprite.active_id = textures.size() - 1;
        }

        else{
            // we need to remove a texture and replace this texture with it
            //for now this will be the first texture or textures[0]
            textures[0] = std::move(texture);
            sprite.active_id = 0;
        }
    }

    else if(sprite.is_tex == true && sprite.active_id != -1){
        
        bool found = false;
        for(int i = 0;i< textures.size();i++){
            if(textures[i]->tex_id == sprite.tex_id){
                found == true;
                sprite.active_id = i;
                break;
            }

        }

        if(found == false){
            // we need to remove a texture and add this instead
            //for now this will be the first texture or textures[0]
            is_change = true;
            auto texture = this->add_texture(sprite.texture_data);
            texture->tex_id = sprite.tex_id;
            textures[0] = std::move(texture);
            sprite.active_id = 0;
        }   

    }


    return 0;
}


std::unique_ptr<Texture> TextureManager::add_texture(const resources::TextureData& data) {
	    return std::make_unique<GlTexture>(data);
}

int TextureManager::bind_textures(){
    if(is_change == true){
    for(int i = 0;i<textures.size();i++){
        GL_ACTIVE_TEXTURE(GL_TEXTURE0 + i);
        textures[i]->bind();
    }
    }
    is_change = false;
}


}}}