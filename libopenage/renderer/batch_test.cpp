#include <cstdlib>
#include "../input/input_manager.h"
#include "opengl/window.h"
#include <eigen3/Eigen/Dense>
#include "../log/log.h"
#include "../error/error.h"
#include "batch_test.h"
#include "opengl/render_target.h"
#include <time.h>
#include <vector>
#include "opengl/texture.h"
#include "resources/texture_data.h"
#include <algorithm>
namespace openage {
namespace renderer {
namespace batch_test{

bool compareSprite(opengl::Sprite_2* i1,opengl::Sprite_2* i2)
{
    return (i1->tex_id < i2->tex_id);
}


void batch_demo(int demo_id,util::Path path){
    
    opengl::GlWindow window("hello world",{1920, 1080});
    auto renderer = window.make_batchrenderer(path);
    
    
    std::vector<opengl::Sprite_2*> sprites;
    
    //char tex_path[] = "/assets/converted/graphics/35.slp.png";
    //bool use_metafile = true;
   //auto tex = resources::TextureData(path / tex_path,use_metafile);
    //resources::TextureInfo tex_info = tex.get_info();
    
    //int num_subtex = tex_info.get_subtexture_count();
    int tex_ids[32] = {2,5,12,689,695,716,779,795,859,855,849,351,343,342,330,339,320,326,354,361,357,363,499,576,578,581,584,591,594,601,600,666};
    for(int j = 0;j<320;j++){
        for(int z = 0;z<180;z++){
        sprites.push_back(new opengl::Sprite_2(path,(float)8*j,(float)8*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites.back()->set_texture(tex_ids[rand()%32],true);
        sprites.back()->set_subtex(rand()%6);
        }
    }

   //std::sort(sprites.begin(),sprites.end(),compareSprite);
   auto vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "/assets/test_shaders/batch.vert.glsl");

	auto fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/batch.frag.glsl");
    auto shade = renderer->add_shader({ vshader_src, fshader_src });


    log::log(INFO <<  RENDERER_BUFFER_SIZE);
    shade->use();
    float zfar = 0.1f;
    float znear = 100.0f;
    Eigen::Matrix4f pers2;
    pers2 << 	2.0f/(1920.0f),0,0,-1.0f,
				0,-2.0f/(1080.0f),0,1.0f,
				0,  0,1.0f,0,
				0.0f,0.0f,  1.0f,  1; 
    
    log::log(INFO <<  "hello 4");
    log::log(INFO <<  sizeof(opengl::Sprite_2)<<"  sprite size");
    float x,y;
    int closed = 0;
    SDL_Event event;
    time_t curr_time,prev_time;
	int frame = 0;
	time(&prev_time);

    

    
    //auto test_texture = renderer->add_texture(tex);
    shade->texture_array();


    while(!closed){
        time(&curr_time);
		frame++;
		if(curr_time-prev_time >= 1){
			prev_time = curr_time;
			log::log(INFO << frame);
			frame = 0;

		}
        //auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"texture_0",test_texture.get());
        
        auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2);
        auto lala = dynamic_cast<opengl::GlUniformInput const*>(new_uniform.get());
        shade->execute_with(lala,nullptr);
        
        renderer->begin();
        for(int k=0;k<sprites.size();k++){
            sprites[k]->x = rand()%1920;
            sprites[k]->y = rand()%1920;
            renderer->submit(*sprites[k]);
        }
        /*renderer->submit(test_sprite);
        renderer->submit(test_sprite_2);
        renderer->submit(test_sprite_3);*/
        renderer->end();
        
        renderer->render();
        SDL_PollEvent(&event);
        if(event.type == SDL_MOUSEMOTION){
            x = event.motion.x;
            y = event.motion.y;
        }

        if(event.type == SDL_QUIT){
            closed = 1;
        }
        //log::log(INFO <<  "Mouse X:"<<x<<"Mouse Y:"<<y);
        SDL_GL_SwapWindow(window.window);
        
    }
    SDL_GL_DeleteContext(window.get_context()->get_raw_context());
    SDL_Quit();
    
  

}
    



}}}