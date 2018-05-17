// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "../error/error.h"
#include "resources/shader_source.h"
#include "resources/texture_data.h"
#include "resources/mesh_data.h"
#include "texture.h"
#include "shader_program.h"
#include "geometry.h"
#include "opengl/window.h"
#include "opengl/context.h"
//#include "perspective.h"
#include "opengl/sprite.h"
#include "opengl/shader_program.h"
#include <time.h>
namespace openage {
namespace renderer {
namespace tests {



void renderer_demo_0(util::Path path) {
	opengl::GlWindow window("openage renderer test", { 1024, 768 } );

	auto renderer = window.make_renderer();
	
	auto vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "/assets/test_shaders/vshader_src.vert.glsl");

	auto fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/fshader_src.frag.glsl");

	auto vshader_display_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "assets/test_shaders/vshader_display_src.vert.glsl");

	auto fshader_display_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/fshader_display_src.frag.glsl");

	auto vshader_alpha = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "assets/test_shaders/alphamask.vert.glsl");

	auto fshader_alpha = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/alphamask.frag.glsl");
	auto size = window.get_size();
	auto shader = renderer->add_shader( { vshader_src, fshader_src } );
	auto shader_display = renderer->add_shader( { vshader_display_src, fshader_display_src } );
	auto alpha_shader = renderer->add_shader({vshader_alpha,fshader_alpha});
	//start of experimental part
	float aspect = (float)size.y/(float)size.x;

	//start of experimental area
	opengl::Sprite sprite;

	//load the texture that we will be using
	auto terrain_texture =  sprite.make_texture(path,"/assets/terrain/textures/g_m02_00_color.png",false,renderer);
	auto paladin = sprite.make_texture(path,"/assets/converted/graphics/795.slp.png",true,renderer);
	auto paladin_2 = sprite.make_texture(path,"/assets/converted/graphics/805.slp.png",true,renderer);
	auto alpha_texture =  sprite.make_texture(path,"/assets/terrain/blends/watershore.png",false,renderer);
	auto shore_texture =  sprite.make_texture(path,"/assets/terrain/textures/g_bch_00_color.png",false,renderer);
	auto water_texture =  sprite.make_texture(path,"/assets/terrain/textures/g_wtr_00_color_1.png",false,renderer);
	auto dust_texture =  sprite.make_texture(path,"/assets/terrain/textures/g_ds3_00_color.png",false,renderer);
	//now to choose which subtexture as well as the transformation matrix. Both of these can
	//be fed in the same function and updated everytime as this does not consume as many resources.
	
	
	
	float left_terr = 0.125f*7;
	float top_terr = 0.125f*1;		
	auto alpha_test = sprite.make_terrain(water_texture,alpha_texture,alpha_shader,aspect,(float)size.y,0,512-64,left_terr,top_terr);
	auto shore_1 = sprite.make_render_obj(shore_texture,true,0,shader,aspect,(float)size.y,0,0);
	auto water_1 = sprite.make_render_obj(water_texture,true,0,shader,aspect,(float)size.y,0,0);
	auto dust = sprite.make_render_obj(dust_texture,true,0,shader,aspect,(float)size.y,600,50);
	auto water_2 = sprite.make_render_obj(shore_texture,true,0,shader,aspect,(float)size.y,0,1024);
	auto water_3 = sprite.make_render_obj(shore_texture,true,0,shader,aspect,(float)size.y,1024,0);
	auto water_4 = sprite.make_render_obj(shore_texture,true,0,shader,aspect,(float)size.y,1024,1024);
	auto elephant = sprite.make_render_obj(paladin,false,0,shader,aspect,(float)size.y,400,450);
	std::vector<Renderable_test> mix_tex; 
	
	
	//mix_tex.push_back(dust);
	
	//mix_tex.push_back(water_2);
	//mix_tex.push_back(water_3);
	//mix_tex.push_back(water_4);
	//mix_tex.push_back(alpha_test);
	/*for(int j = 0; j<4;j++){
	for(int i=-3;i<2;i++){
		mix_tex.push_back(sprite.make_render_obj(water_texture,true,0,shader,aspect,(float)size.y,512*j,512*i));
		mix_tex.push_back(sprite.make_render_obj(shore_texture,true,0,shader,aspect,(float)size.y,512*j,512*i));
		mix_tex.push_back(sprite.make_render_obj(dust_texture,true,0,shader,aspect,(float)size.y,512*j,512*i));
	}
	}*/
	for(int z = 0;z<10000;z++){
		
		mix_tex.push_back(sprite.make_render_obj(paladin,false,rand()%20,shader,aspect,(float)size.y,rand()%1920,rand()%1920));
		//mix_tex.push_back(sprite.make_render_obj(paladin_2,false,15,shader,aspect,(float)size.y,rand()%1024,rand()%1024));
			
	}
	//mix_tex.push_back(elephant);
	log::log(INFO << "what is path "<<path);
	log::log(INFO << "Size of the Window "<<size.x<<"X"<<size.y);
	auto color_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::depth24));
	//one of the targets
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );
	
	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", color_texture.get());

	resources::TextureData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());

	Renderable_test display_obj {
		color_texture_uniform,
		quad,
		false,
		false,
	};

	/*RenderPass_test pastu{
		{terrain_1,terrain_2,terrain_3,terrain_4,terrain_5,terrain_6,terrain_7,terrain_8,terrain_9,paladin_1},//,test_obj5,test_obj6,test_obj7},
		fbo.get(),
	};*/
	/*RenderPass_test alpha_pass{
		{water_1,water_2,water_3,water_4,alpha_test},
		fbo.get(),
	};*/
	RenderPass_test alpha_pass{
		mix_tex,
		fbo.get(),
	};
	RenderPass_test render_main{
		{display_obj},
		renderer->get_display_target(),
	};


	//glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	// what is this
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.add_mouse_button_callback([&] (SDL_MouseButtonEvent const& ev) {
			auto x = ev.x;
			auto y = ev.y;

			log::log(INFO << "Clicked at location (" << x << ", " << y << ")");
			if (!texture_data_valid) {
				id_texture_data = id_texture->into_data();
				texture_data_valid = true;
			}
			auto id = id_texture_data.read_pixel<uint32_t>(x, y);
			log::log(INFO << "Id-texture-value at location: " << id);
			if (id == 0) {
				//no renderable at given location
				log::log(INFO << "Clicked at non existent object");
				return;
			}
			id--; //real id is id-1
			log::log(INFO << "Object number " << id << " clicked.");
			renderer->display_into_data().store(path / "/assets/screen.png");
		} );

	window.add_resize_callback([&] (coord::window new_size) {
			// Calculate projection matrix
			float aspectRatio = float(new_size.x)/float(new_size.y);
			float xScale = 1.0/aspectRatio;

			Eigen::Matrix4f pmat;
			pmat << 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1;

			// resize fbo
			color_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::rgba8));
			id_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::r32ui));
			depth_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::depth24));
			fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );
			texture_data_valid = false;

			shader_display->update_uniform_input(color_texture_uniform.get(), "color_texture", color_texture.get(), "proj", pmat);
			alpha_pass.target = fbo.get();
		} );
	time_t curr_time,prev_time;
	int frame = 0;
	time(&prev_time);
	while (!window.should_close()) {
		time(&curr_time);
		frame++;
		if(curr_time-prev_time >= 1){
			prev_time = curr_time;
			log::log(INFO << frame);
			frame = 0;
		}
		
		renderer->render_test(alpha_pass);
		renderer->render_test(render_main);
		window.update();
		window.get_context()->check_error();
	}
}


void renderer_demo(int demo_id, util::Path path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	default:
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
		break;
	}
}

}}} // namespace openage::renderer::tests
