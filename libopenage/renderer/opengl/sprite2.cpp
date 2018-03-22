#include "sprite2.h"
#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <map>
#include <eigen3/Eigen/Dense>
#include "../../log/log.h"
#include "../../error/error.h"
#include "../resources/shader_source.h"
#include "../resources/texture_data.h"
#include "../resources/mesh_data.h"
#include "context.h"
//#include "texture.h"
#include "shader_program.h"
#include "../../util/path.h"
#include "../renderer.h"
#include "texture.h"

int Sprite::make_texture(util::Path path,char tex_path[],bool use_metafile){
     
     texture_data = resources::TextureData(path / tex_path,use_metafile);
     sprite_texture = renderer->add_texture(tex);
     
     return 1;
}

Sprite::sprite(float aspect,int Y_screen,std::shared_ptr<ShaderProgram> shader,std::shared_ptr<Renderer> renderer){
    Sprite::aspect = aspect;
    Sprite::Y_screen = Y_screen;
    Sprite::shader = shader;
    Sprite::renderer = renderer;
    }