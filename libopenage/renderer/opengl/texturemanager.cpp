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
        bool is_loaded = false;// this is for finding if a texture is loaded not for checking if bound
        for(uint i = 0;i < textures.size();i++){
            if(textures[i].tex->tex_id == sprite.tex_id){
                is_loaded = true;
                sprite.vec_id = i;
                log::log(INFO << "found true "<<sprite.tex_id);
                break;
            }
        }

        if(is_loaded == true){
            bool is_bound = false;
            for(uint i = 0;i< current_textures.size();i++){
                if(current_textures[i] == sprite.vec_id){
                    is_bound = true;
                    sprite.active_id = i;
                    //log::log(INFO << "is_tex and not active id found true "<<sprite.tex_id);
                    break;
                }
            }
            if(is_bound == false){
                if(current_textures.size() < 32){
                    current_textures.push_back(sprite.vec_id);
                    sprite.active_id = current_textures.size() - 1;
                }

                else{
                    // we need to remove a texture and replace this texture with it
                    //for now this will be the first texture or textures[0]
                    //log::log(INFO<<"fatal error in loaded but not bounded");
                    current_textures[0] = sprite.vec_id;
                    sprite.active_id = 0;
                }
            }

        }
        else if(is_loaded == false){
            log::log(INFO << "found false "<<sprite.tex_id);
            is_change = true;
           
            auto texture = this->add_texture(sprite.tex_id,sprite.meta,sprite.is_terrain); // load the texture
            texture.tex->tex_id = sprite.tex_id;
            textures.push_back(texture);
            sprite.vec_id = textures.size() - 1;
            //log::log(INFO << "just after the shared ptr fuckup");

            if(current_textures.size() < 32){
                current_textures.push_back(sprite.vec_id);
                sprite.active_id = current_textures.size() - 1;
            }

            else{
                // we need to remove a texture and replace this texture with it
                //for now this will be the first texture or textures[0]
                //log::log(INFO<<"fatal error loaded false");
                current_textures[0] = sprite.vec_id;
                sprite.active_id = 0;
            }

        }
    }

    else if(sprite.is_tex == true && sprite.active_id != -1){
        //log::log(INFO << "is tex true and active id not -1");
        //log::log(INFO << "is_tex and not active id "<<sprite.tex_id);
        bool is_bound = false;
        for(uint i = 0;i< current_textures.size();i++){
            if(current_textures[i] == sprite.vec_id){
                is_bound = true;
                sprite.active_id = i;
                //log::log(INFO << "is_tex and not active id found true "<<sprite.tex_id);
                break;
            }

        }

        if(is_bound == false){
            // we need to remove a texture and add this instead
            //for now this will be the first texture or textures[0]
            //log::log(INFO << "is_tex and not active id found false"<<sprite.tex_id);
            is_change = true;
            if(current_textures.size() < 32){
                current_textures.push_back(sprite.vec_id);
                sprite.active_id = current_textures.size() - 1;
            }
            else{
            //log::log(INFO<<"fatal error loaded not bounded repeat");
            current_textures[0] = sprite.vec_id;
            sprite.active_id = 0;
            }
        }   

    }


    return 0;
}


std::unique_ptr<Texture> TextureManager::add_tex(resources::TextureData& data) {
	    return std::make_unique<GlTexture>(data);
}

int TextureManager::bind_textures(){
    //if(is_change == true){
    for(uint i = 0;i<current_textures.size();i++){
        glActiveTexture(GL_TEXTURE0 + i);
        textures[current_textures[i]].tex->bind();
    }
    //}
    //is_change = false;
}


TextureStruct TextureManager::add_texture(int tex_id,bool meta_file, bool is_terrain){
    std::string path;
    if(!is_terrain)
        path = "/assets/converted/graphics/"+ std::to_string(tex_id) +".slp.png";
    else{
        //path = "/assets/terrain/textures/" + std::to_string(tex_id - 6000) + ".png";
        path = "/assets/converted/terrain/" + std::to_string(tex_id) + ".slp.png";
    }
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
    auto size = textures[temp].data.get_info().get_subtexture_size(sprite.subtex);
    float left = std::get<0>(sprite_coord);
  	float right = std::get<1>(sprite_coord);
  	float bottom = std::get<2>(sprite_coord);
  	float top = std::get<3>(sprite_coord);
    sprite.left = left;
    sprite.right = right;
    sprite.top = top;
    sprite.bottom = bottom;
    sprite.w = size.first;
    sprite.h = size.second;
}



bool TextureManager::check_texture(Sprite_2& sprite){
    if(sprite.is_tex == false){
        return false;
    }


    else if(sprite.is_tex == true && sprite.active_id == -1){
        //log::log(INFO << "which sprite is: tex_id in true and -1 "<<sprite.tex_id);
        bool is_loaded = false;// this is for finding if a texture is loaded not for checking if bound
        for(uint i = 0;i < textures.size();i++){
            if(textures[i].tex->tex_id == sprite.tex_id){
                is_loaded = true;
                break;
            }
        }

        if(is_loaded == true){
            bool is_bound = false;
            for(uint i = 0;i< current_textures.size();i++){
                if(current_textures[i] == sprite.vec_id){
                    is_bound = true;
                    return false;
                    break;
                }
            }
            if(is_bound == false){
                if(current_textures.size() >= 32)
                    return true;
                else 
                    return false;
            }

        }
        else if(is_loaded == false){
            if(current_textures.size() >= 32)
                return true;
            else 
                return false;
        }
    }

    else if(sprite.is_tex == true && sprite.active_id != -1){
        bool is_bound = false;
        for(uint i = 0;i< current_textures.size();i++){
            if(current_textures[i] == sprite.vec_id){
                is_bound = true;
                return false;
                break;
            }
        }
        if(is_bound == false){
            if(current_textures.size() >= 32)
                return true;
            else 
                return false;
        }

    }
}

}}}