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

void batch_demo(int demo_id,util::Path path){

    opengl::GlWindow window("hello world",{1024, 768});

    std::vector<opengl::Sprite_2*> sprites;

    auto renderer = window.make_vertexrenderer(path);
    auto renderer_2 = window.make_vertexrenderer(path);
    auto vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "/assets/test_shaders/not_batch.vert.glsl");

	auto fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/not_batch.frag.glsl");
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
    Eigen::Matrix4f dimet; /// This is the DIMETRIC Perspective. Used in age of empires (Reference: https://gamedev.stackexchange.com/questions/16746/what-is-the-name-of-perspective-of-age-of-empires-ii)
	    dimet << 	1.0,1,0,0,
	            	0.5,-0.5,0.75,0,
	            	0,0,0,0,
	            	0,0,0,1;
    log::log(INFO <<  "hello 4");
    log::log(INFO <<  sizeof(opengl::Sprite_2)<<"  sprite size");
    float x,y;
    int closed = 0;
    SDL_Event event;
    clock_t curr_time,prev_time,update_time;
	int frame = 0,j = 0,m=0;
	prev_time = clock();
  	update_time = clock();


    opengl::Sprite_2 test_sprite(100.0f,100.0f,100.0f,100.0f,1.0f,0.0f,1.0f,1.0f);
    for(int i = 0;i<1000;i++){
        sprites.push_back(new opengl::Sprite_2(rand()%1920,rand()%1080,100.0f,100.0f,(rand()%1000)/1000.0f,(rand()%1000)/1000.0f,(rand()%1000)/1000.0f,1.0f));
    }

    auto tex_data = resources::TextureData(path / "/assets/terrain/textures/g_m02_00_color.png",false);
    auto testore = renderer->add_texture(tex_data);
    glActiveTexture(GL_TEXTURE0);
    testore->bind();
    shade->texture_array(1);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // important to remove the black square around the textures or the transperent area.
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
        
    
    while(!closed){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		frame++;
		if((float)(clock()-prev_time)/CLOCKS_PER_SEC >= 1.0){
      		prev_time = clock();
			log::log(INFO <<" frames " << frame);
			frame = 0;
		}

        //auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2);//,"dimet",dimet);
        auto new_uniform = shade->new_uniform_input("ortho",pers2);//,"dimet",dimet);
        
        auto lala = dynamic_cast<opengl::GlUniformInput const*>(new_uniform.get());
        shade->execute_with(lala,nullptr);
        renderer->begin();
        for(int k=0;k<sprites.size();k++){
			sprites[k]->x = rand()%1920;
			sprites[k]->y = rand()%1080;
            renderer->submit(*(sprites[k]));
		}
        renderer->submit(test_sprite);
        renderer->end();
        renderer->render();

        /*renderer_2->begin();
        for(int k=0;k<sprites.size();k++){
			sprites[k]->x = rand()%1920;
			sprites[k]->y = rand()%1080;
            renderer_2->submit(*(sprites[k]));
		}
        renderer_2->submit(test_sprite);
        renderer_2->end();
        renderer_2->render();*/


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