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
    auto renderer_2 = window.make_batchrenderer(path);
    auto renderer_3 = window.make_batchrenderer(path);
    auto renderer_4 = window.make_batchrenderer(path);
    auto renderer_5 = window.make_batchrenderer(path);
    std::vector<opengl::Sprite_2*> sprites;
    std::vector<opengl::Sprite_2*> sprites_2;
    std::vector<opengl::Sprite_2*> terrain;
    std::vector<opengl::Sprite_2*> trees;
    std::vector<opengl::Sprite_2*> houses;
    int tex_ids_2[32] = {2,5,12,689,695,716,779,795,859,855,849,351,343,342,330,339,320,326,354,361,357,363,499,576,578,581,584,591,594,601,600,805};
    //int tex_ids[32] = {21,22,23,24,33,34,35,36,90,91,92,93,102,103,104,105,130,131,132,133,142,143,144,145,278,279,280,281,302,303,304,305};
    int tex_ids[40] = {2,5,12,689,695,716,779,795,859,855,849,351,343,342,330,339,320,326,354,361,357,363,499,576,578,581,584,591,594,601,600,805,61,64,67,71,171,179,181,186};
    for(int j = 0;j<20;j++){
        for(int z = 0;z<10;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites.push_back(new opengl::Sprite_2(path,(float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites.back()->set_texture(tex_ids[(10*j + z)%32],true);
        sprites.back()->set_subtex(0);
        }
    }

    for(int j = 0;j<20;j++){
        for(int z = 0;z<10;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites_2.push_back(new opengl::Sprite_2(path,(float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites_2.back()->set_texture(tex_ids_2[(10*j + z)%32],true);
        sprites_2.back()->set_subtex(0);
        }
    }
    
    for(int j = -3;j<2;j++){
        for(int z = 0;z<4;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        terrain.push_back(new opengl::Sprite_2(path,(float)(512*(z)),(float)(512*j),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        terrain.back()->set_terrain(6009 + rand()%32);
        terrain.push_back(new opengl::Sprite_2(path,(float)(512*(z)),(float)(512*j),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        terrain.back()->set_terrain(6009 + rand()%32);
        }
    }

    for(int j = 0;j<40;j++){
        for(int z = 0;z<10;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        trees.push_back(new opengl::Sprite_2(path,(float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        trees.back()->set_texture(4731,true);
        trees.back()->set_subtex(rand()%6);
        }
    }

    for(int j = 0;j<20;j++){
        for(int z = 0;z<1;z++){
        //sprites.push_back(new opengl::Sprite_2(path,(float)60*j,(float)60*z,(float)6,(float)6,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        houses.push_back(new opengl::Sprite_2(path,(float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        houses.back()->set_texture(2235,true);
        houses.back()->set_subtex(rand()%3);
        }
    }

    opengl::Sprite_2 archery(path,900.0f,100.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    archery.set_texture(21,true);
    opengl::Sprite_2 castle(path,100.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    castle.set_texture(305,false);
    opengl::Sprite_2 blacksmith(path,100.0f,700.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    blacksmith.set_texture(92,true);
    opengl::Sprite_2 barrack(path,200.0f,50.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    barrack.set_texture(145,true);
    opengl::Sprite_2 monastery(path,300.0f,700.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    monastery.set_texture(280,true);
    opengl::Sprite_2 market(path,500.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    market.set_texture(818,true);
    opengl::Sprite_2 workshop(path,1000.0f,500.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    workshop.set_texture(956,true);
    opengl::Sprite_2 viking_ship(path,1000.0f,800.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    viking_ship.set_texture(699,true);
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
            //sprites[k]->x = rand()%1920;
            //sprites[k]->y = rand()%1080;
            sprites[k]->set_subtex(j%5);
            
        }
            viking_ship.set_subtex(j%9);
            j++;
		}
        //auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"texture_0",test_texture.get());
        
        auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"dimet",dimet);
        auto lala = dynamic_cast<opengl::GlUniformInput const*>(new_uniform.get());
        shade->execute_with(lala,nullptr);
        
        renderer->begin();
        
        for(int k=0;k<terrain.size();k++){
            renderer->submit(*(terrain[k]));
        }
        renderer->end();
        renderer->render();
        renderer->clear_textures();
        renderer_2->begin();
        renderer_2->submit(archery);
        renderer_2->submit(castle);
        renderer_2->submit(blacksmith);
        renderer_2->submit(workshop);
        renderer_2->submit(market);
        renderer_2->submit(barrack);
        renderer_2->submit(monastery);
        renderer_2->submit(viking_ship);
        for(int k=0;k<houses.size();k++){
            renderer_2->submit(*(houses[k]));
        }
        renderer_2->end();           
        renderer_2->render();
        renderer_2->clear_textures();

        renderer_3->begin();
        for(int k=0;k<trees.size();k++){
            //sprites[k]->x = rand()%1920;
            //sprites[k]->y = rand()%1920;
            renderer_3->submit(*(trees[k]));
            
        }
        renderer_3->end();
        renderer_3->render();
        renderer_4->begin();

        for(int k=0;k<sprites.size();k++){
            //sprites[k]->x = rand()%1920;
            //sprites[k]->y = rand()%1920;
            renderer_4->submit(*(sprites[k]));
            
        }
        renderer_4->end();
        renderer_4->render();
        renderer_4->clear_textures();

        /*renderer_5->begin();

        for(int k=0;k<sprites_2.size();k++){
            //sprites[k]->x = rand()%1920;
            //sprites[k]->y = rand()%1920;
            renderer_5->submit(*(sprites_2[k]));
            
        }
        renderer_5->end();
        renderer_5->render();
        renderer_5->clear_textures();*/
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