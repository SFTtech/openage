#include "sprite.h"
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
namespace openage{
    namespace renderer{
    namespace opengl{

Renderable_test Sprite::make_render_obj(sprite_texture texture,bool pers_bool,int subtex,std::shared_ptr<ShaderProgram> shader,float X,float Y){

  auto info_tex = texture.data.get_info();
  auto image_info = info_tex.get_subtexture_size(subtex);
  float image_aspect = (float)image_info.first/(float)image_info.second;
  auto transform = make_transform(pers_bool,Eigen::Vector3f(X,Y,0));
  	auto unif_in1 = shader->new_uniform_input(
  		"mvp", transform.matrix(),
  		"u_id", 2u,
  		"tex", texture.tex_handle.get()
  	);
  auto quad = this->get_subtex(texture.data,subtex, X,Y);
  return {unif_in1,quad,true,true};
}

std::shared_ptr<UniformInput> Sprite::get_uniform(sprite_texture texture,bool pers_bool,std::shared_ptr<ShaderProgram> shader,float X,float Y){
  auto transform = make_transform(pers_bool,Eigen::Vector3f(X,Y,0));
  	auto unif_in1 = shader->new_uniform_input(
  		"mvp", transform.matrix(),
  		"u_id", 2u,
  		"tex", texture.tex_handle.get()
  	);

    return unif_in1;
}

std::shared_ptr<UniformInput> Sprite::get_uniform2(sprite_texture texture,bool pers_bool,std::shared_ptr<ShaderProgram> shader,float X,float Y){
  	auto unif_in1 = shader->new_uniform_input(
  		"pos", Eigen::Vector2f(X,Y),
  		"u_id", 2u,
  		"tex", texture.tex_handle.get()
  	);

    return unif_in1;
}


std::shared_ptr<Geometry> Sprite::get_subtex(resources::TextureData tex,int subtex,float X,float Y){
    resources::TextureInfo tex_info = tex.get_info();
  	size_t count = tex_info.get_subtexture_count();
  	//log::log(INFO << "Number of sprites in spritesheet : "<<count);
  	std::tuple<float, float, float, float> sprite_coord = tex_info.get_subtexture_coordinates(subtex);
  	//log::log(INFO << "Coordinate of Sprite "<<subtex<<" is : "<< std::get<0>(sprite_coord)<<"  "<<std::get<1>(sprite_coord)<<"  "<<std::get<2>(sprite_coord)<<"  "<<std::get<3>(sprite_coord));
  	float left = std::get<0>(sprite_coord);
  	float right = std::get<1>(sprite_coord);
  	float bottom = std::get<2>(sprite_coord);
  	float top = std::get<3>(sprite_coord);
    float position[2] = {X,Y};
    int size_img[2] = {tex_info.get_subtexture_size(subtex).first,tex_info.get_subtexture_size(subtex).second}; 
    std::array<float, 16> quad_data = { {
  			-100.0f, 100.0f, left, bottom,//bottom left
  			-100.0f, -100.0f, left, top,//top left
  			100.0f, 100.0f, right, bottom,//bottom right
  			100.0f, -100.0f, right, top//top right
  		} };
    std::array<float, 16> quad_data2 = { {
  			position[0], position[1], left, bottom,//bottom left
  			position[0], position[1] + size_img[1], left, top,//top left
  			position[0] + size_img[0], position[1], right, bottom,//bottom right
  			position[0] + size_img[0], position[1] + size_img[1], right, top//top right
  		} };
      std::array<float, 16> quad_data3 = { {
  			0, 0, left, bottom,//bottom left
  			0, size_img[1], left, top,//top left
  			size_img[0], 0, right, bottom,//bottom right
  			size_img[0], size_img[1], right, top//top right
  		} };
    auto mesh = resources::MeshData::make_quad(quad_data3);
    return std::make_shared<GlGeometry>(mesh);
  }
Eigen::Transform<float, 3, 32> Sprite::make_transform(bool pers_bool,Eigen::Vector3f center_coord){
  float zfar = 0.1f;                 // for now these are constant
  float znear = 100.0f;
  auto transform = Eigen::Affine3f::Identity();
	//transform.prescale(Eigen::Vector3f(0.5, 0.5,0.0f));
  transform.pretranslate(center_coord);
  Eigen::Matrix4f dimet; /// This is the DIMETRIC Perspective. Used in age of empires (Reference: https://gamedev.stackexchange.com/questions/16746/what-is-the-name-of-perspective-of-age-of-empires-ii)
	    dimet << 	1.0,-1,0,0,
	            	0.5,0.5,0.75,0,
	            	0,0,0,0,
	            	0,0,0,1;

	Eigen::Matrix4f pers,pers2;  //this is the Perspective Matrix, Currently Used to normalise the aspect ratio. Read about UV coordinates to see why this is needed.
	pers << 	(float)1.0,0,0,0,
						0,1.0,0,0,
						0,  0,-2*(float)(zfar*znear)/(zfar-znear),0,
					  0,0,  -1*(float)(zfar+znear)/(zfar-znear),  1;

  pers2 << 	2.0f/(1366.0f),0,0,-1.0f,
						0,-2.0f/(768.0f),0,1.0f,
						0,  0,1.0,0,
					  0.0f,0.0f, 1.0,  1;  
              Eigen::Transform<float, 3, 32> transform_temp;
  if(pers_bool){
  transform_temp = pers2*dimet*transform;
}
  else{
  transform_temp = pers2*transform;
  }
  return transform_temp;

  }

   sprite_texture Sprite::make_texture(util::Path path,char tex_path[],bool use_metafile,std::shared_ptr<Renderer> renderer){
     
     auto tex = resources::TextureData(path / tex_path,use_metafile);
     auto gltex = renderer->add_texture(tex);
     return {std::move(gltex),tex};
    }

    Renderable_test Sprite::make_terrain(sprite_texture base,sprite_texture alpha_mask,std::shared_ptr<ShaderProgram> alpha_shader,float aspect,float Y_screen,float X,float Y,float top_terr,float left_terr){
    float left = left_terr;
    float right = left_terr+0.125f;
    float top = top_terr;
    float bottom = top_terr+0.125f;
      	static const std::array<float, 24> alpha_data = { {
		-1.0f, 1.0f,left, bottom,left, bottom,
		-1.0f, -1.0f, left, top,left, top,
		1.0f, 1.0f, right, bottom,right, bottom,
		1.0f, -1.0f, right, top, right, top
	  } };
    auto info_tex = base.data.get_info();
    auto image_info = info_tex.get_subtexture_size(0);
    float image_aspect = (float)image_info.first/(float)image_info.second;
    log::log(INFO << "this is image aspect : "<<image_aspect);
    //auto transform = make_transform((float)image_info.second/(float)Y_screen,true,Eigen::Vector3f(X,Y,0),(image_aspect)*aspect);
    auto transform = make_transform(true,Eigen::Vector3f(X,Y,0));
    
    auto alpha_uniform = alpha_shader->new_uniform_input(
		"base_texture",base.tex_handle.get(),
		"mask_texture",alpha_mask.tex_handle.get(),
		"mvp_matrix",transform.matrix()
		//"show_mask",true
	);
  auto mesh_alpha = resources::MeshData::make_alpha(alpha_data);
	auto quad = std::make_shared<GlGeometry>(mesh_alpha);
  return {alpha_uniform,quad,true,true};
   }
  }
  }
}
