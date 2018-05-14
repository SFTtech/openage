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
    std::vector<opengl::Sprite_2*> terrain;
    //int tex_ids[32] = {2,5,12,689,695,716,779,795,859,855,849,351,343,342,330,339,320,326,354,361,357,363,499,576,578,581,584,591,594,601,600,805};
    //int tex_ids[32] = {21,22,23,24,33,34,35,36,90,91,92,93,102,103,104,105,130,131,132,133,142,143,144,145,278,279,280,281,302,303,304,305};
    int tex_ids[40] = {21,22,23,24,33,34,35,36,90,91,92,93,102,103,104,105,130,131,132,133,142,143,144,145,278,279,280,281,302,303,304,305,376,716,779,795,859,855,849,351};
    for(int j = 0;j<10;j++){
        for(int z = 0;z<10;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites.push_back(new opengl::Sprite_2(path,(float)(rand()%1920),(float)(rand()%1920),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites.back()->set_texture(tex_ids[rand()%40],true);
        sprites.back()->set_subtex(0);
        }
    }
    
    for(int j = -20;j<23;j++){
        for(int z = -1;z<40;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        terrain.push_back(new opengl::Sprite_2(path,(float)(48*(z)),(float)(24*z + 48*j),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        terrain.back()->set_terrain(15000 + rand()%6); //+rand()%6);
        terrain.back()->set_subtex(rand()%6);
        }
    }


    opengl::Sprite_2 test_sprite(path,400.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    test_sprite.set_texture(805,true);
    opengl::Sprite_2 test_sprite_2(path,500.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    test_sprite_2.set_texture(35,false);
    opengl::Sprite_2 test_sprite_3(path,500.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    test_sprite_3.set_texture(792,true);
    opengl::Sprite_2 viking_ship(path,400.0f,800.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    viking_ship.set_texture(699,true);
    opengl::Sprite_2 terrain_sprite_1(path,0.0f,0.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    terrain_sprite_1.set_terrain(15001);
    opengl::Sprite_2 terrain_sprite_2(path,48.0f,24.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    terrain_sprite_2.set_terrain(15001);
    opengl::Sprite_2 terrain_sprite_3(path,48.0f,-24.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    terrain_sprite_3.set_terrain(15001);
    opengl::Sprite_2 terrain_sprite_4(path,96.0f,0.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    terrain_sprite_4.set_terrain(15001);
   std::sort(sprites.begin(),sprites.end(),compareSprite);
   std::sort(terrain.begin(),terrain.end(),compareSprite);
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
    time_t curr_time,prev_time;
	int frame = 0,j = 0;
	time(&prev_time);

    

    
    //auto test_texture = renderer->add_texture(tex);
    shade->texture_array();

    glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // important to remove the black square around the textures or the transperent area.
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
    while(!closed){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        time(&curr_time);
		frame++;
		if(curr_time-prev_time >= 1){
			prev_time = curr_time;
			log::log(INFO << frame);
			frame = 0;
            for(int k=0;k<sprites.size();k++){
            sprites[k]->x = rand()%1920;
            sprites[k]->y = rand()%1080;
            
        }
            viking_ship.set_subtex(j%9);
            j++;
		}
        //auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"texture_0",test_texture.get());
        
        auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"dimet",dimet);
        auto lala = dynamic_cast<opengl::GlUniformInput const*>(new_uniform.get());
        shade->execute_with(lala,nullptr);
        
        renderer->begin();
        
        /*for(int k=0;k<terrain.size();k++){
            //sprites[k]->x = rand()%1920;
            //sprites[k]->y = rand()%1920;
            renderer->submit(*(terrain[k]));
            
        }
        renderer->end();
        renderer->render();
        renderer->begin();
        renderer->clear_textures();*/
        for(int k=0;k<sprites.size();k++){
            //sprites[k]->x = rand()%1920;
            //sprites[k]->y = rand()%1920;
            renderer->submit(*(sprites[k]));
            
        }
        //renderer->submit(terrain_sprite_1);
        //renderer->submit(terrain_sprite_2);
        //renderer->submit(terrain_sprite_3);
        //renderer->submit(terrain_sprite_4);
        /*renderer->submit(test_sprite);
        renderer->submit(test_sprite_2);
        renderer->submit(test_sprite_3);*/
        //renderer->submit(viking_ship);
        renderer->end();
        renderer->render();
        //renderer->clear_textures();
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