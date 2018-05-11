#include "texturemanager.h"


namespace openage{ 
namespace renderer{
namespace opengl{

    TextureManager::TextureManager(util::Path& path){
        this->root = path;
    }

int TextureManager::get_activeID(Sprite_2& sprite){
    //log::log(INFO << "A");
    if(sprite.is_tex == false){
        //log::log(INFO << "is_tex == false");
        sprite.active_id = -1;
        return -1;
    }
    else if(sprite.is_tex == true && sprite.active_id == -1){
        //log::log(INFO << "which sprite is: tex_id in true and -1 "<<sprite.tex_id);
        bool found = false;
        for(uint i = 0;i< textures.size();i++){
            if(textures[i].tex->tex_id== sprite.tex_id){
                found = true;
                sprite.active_id = i;
                log::log(INFO << "found true "<<sprite.tex_id);
                break;
            }

        }

        if(found == false){
            log::log(INFO << "found false "<<sprite.tex_id);
        is_change = true;
        auto texture = this->add_texture(sprite.tex_id,sprite.meta);
        //log::log(INFO << "just after the shared ptr fuckup");
        texture.tex->tex_id = sprite.tex_id;

        if(textures.size() < 32){
            textures.push_back(texture);
            sprite.active_id = textures.size() - 1;
        }

        else{
            // we need to remove a texture and replace this texture with it
            //for now this will be the first texture or textures[0]
            textures[0] = texture;
            sprite.active_id = 0;
        }

        }
    }

    else if(sprite.is_tex == true && sprite.active_id != -1){
        //log::log(INFO << "is tex true and active id not -1");
        //log::log(INFO << "is_tex and not active id "<<sprite.tex_id);
        bool found = false;
        for(uint i = 0;i< textures.size();i++){
            if(textures[i].tex->tex_id == sprite.tex_id){
                found = true;
                sprite.active_id = i;
                //log::log(INFO << "is_tex and not active id found true "<<sprite.tex_id);
                break;
            }

        }

        if(found == false){
            // we need to remove a texture and add this instead
            //for now this will be the first texture or textures[0]
            log::log(INFO << "is_tex and not active id found false"<<sprite.tex_id);
            is_change = true;
            auto texture = this->add_texture(sprite.tex_id,sprite.meta);
            texture.tex->tex_id = sprite.tex_id;
            textures[0] = texture;
            sprite.active_id = 0;
        }   

    }


    return 0;
}


std::unique_ptr<Texture> TextureManager::add_tex(resources::TextureData& data) {
	    return std::make_unique<GlTexture>(data);
}

int TextureManager::bind_textures(){
    if(is_change == true){
    for(uint i = 0;i<textures.size();i++){
        glActiveTexture(GL_TEXTURE0 + i);
        textures[i].tex->bind();
    }
    }
    is_change = false;
}


TextureStruct TextureManager::add_texture(int tex_id,bool meta_file){
    std::string path = "/assets/converted/graphics/"+ std::to_string(tex_id) +".slp.png";
    resources::TextureData data(root/path,meta_file);
    auto texture = this->add_tex(data);
    return {data,std::move(texture)};
}

void TextureManager::getUV(Sprite_2& sprite){
    int temp;
    for(int i = 0;i< textures.size();i++){
        if(sprite.tex_id == textures[i].tex->tex_id){
            temp = i;
            break;
        }
    }
    
    std::tuple<float, float, float, float> sprite_coord = textures[temp].data.get_info().get_subtexture_coordinates(sprite.subtex);
    float left = std::get<0>(sprite_coord);
  	float right = std::get<1>(sprite_coord);
  	float bottom = std::get<2>(sprite_coord);
  	float top = std::get<3>(sprite_coord);
    sprite.left = left;
    sprite.right = right;
    sprite.top = top;
    sprite.bottom = bottom;
}


}}}