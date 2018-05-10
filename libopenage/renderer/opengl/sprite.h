#pragma once
#include "../renderer.h"
#include <memory>
#include <eigen3/Eigen/Dense>
#include "../texture.h"
#include "../geometry.h"
#include "shader_program.h"
namespace openage{
    namespace renderer{
      namespace opengl{

struct sprite_texture {
  std::shared_ptr<Texture> tex_handle;
  resources::TextureData data;
};

class Sprite {

public:
  int image_size[2] = {0,0};
  //std::shared_ptr<UniformInput> make_unif(util::Path path,char tex_path[],std::shared_ptr<ShaderProgram> shader,std::shared_ptr<Renderer> renderer);
  Renderable_test create(util::Path path,bool pers_bool,char tex_path[],int subtex,bool use_metafile,std::shared_ptr<ShaderProgram> shader,std::shared_ptr<Renderer> renderer,float aspect,float Y_screen,Eigen::Vector3f center_coord);
  /// this returns the geometry
  std::shared_ptr<Geometry> get_subtex(resources::TextureData tex,int subtex,float X,float Y);
  //creates the transformation matrix
  Eigen::Transform<float, 3, 32> make_transform(float scale,bool pers_bool,Eigen::Vector3f center_coord,float aspect);
  
  //the very efficient remake
  //this makes a reusable texture which contains the data as well as the handle
  //to the uploaded texture
  sprite_texture make_texture(util::Path path,char tex_path[],bool use_metafile,std::shared_ptr<Renderer> renderer);
  
  Renderable_test make_render_obj(sprite_texture texture,bool pers_bool,int subtex,std::shared_ptr<ShaderProgram> shader,float aspect,float Y_screen,float X,float Y);
  Renderable_test make_terrain(sprite_texture base,sprite_texture alpha_mask,std::shared_ptr<ShaderProgram> alpha_shader,float aspect,float Y_screen,float X,float Y,float top_terr,float left_terr);


};

}}}
