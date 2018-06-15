#include <cstdlib>
#include "../input/input_manager.h"
#include "opengl/window.h"
#include "../log/log.h"
#include "../error/error.h"
#include "batch_test.h"
#include "opengl/render_target.h"
#include <time.h>
#include <vector>
#include "opengl/terrainmanager.h"
//#include "opengl/texturearray.h"
#include "resources/texture_data.h"
#include <algorithm>
#include "../util/externalprofiler.h"
namespace openage {
namespace renderer {
namespace batch_test{

bool compareSprite(opengl::Sprite_2* i1,opengl::Sprite_2* i2)
{
    return (i1->tex_id < i2->tex_id);
}

void batch_demo(int demo_id,util::Path path){

    util::ExternalProfiler external_profiler;

    opengl::GlWindow window("hello world",{1920, 1080});

    std::vector<opengl::Sprite_2*> sprites;
    std::vector<opengl::Sprite_2*> sprites_2;
    std::vector<opengl::Sprite_2*> terrains;
    std::vector<opengl::Sprite_2*> trees;

    auto renderer = window.make_batchrenderer(path);
    auto renderer_2 = window.make_batchrenderer(path);
    auto renderer_3 = window.make_batchrenderer(path);
    auto vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "/assets/test_shaders/batch.vert.glsl");

	auto fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/batch.frag.glsl");
    auto shade = renderer->add_shader({ vshader_src, fshader_src });

    auto vshader_batch = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		path / "/assets/test_shaders/not_batch.vert.glsl");

	auto fshader_batch = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		path / "assets/test_shaders/not_batch.frag.glsl");
    auto shadebatch = renderer->add_shader({ vshader_batch, fshader_batch });

    log::log(INFO <<  RENDERER_BUFFER_SIZE);
    float zfar = 0.1f;
    float znear = 100.0f;
    Eigen::Matrix4f pers2;
    pers2 << 	2.0f/(1920.0f),0,0,-1.0f,
				0,-2.0f/(1080.0f),0,1.0f,
				0,  0,1.0f,0,
				0.0f,0.0f,  1.0f,  1;
    Eigen::Matrix4f dimet; /// This is the DIMETRIC Perspective. Used in age of empires (Reference: https://gamedev.stackexchange.com/questions/16746/what-is-the-name-of-perspective-of-age-of-empires-ii)
	    dimet << 	1.0,1.0,0,0,
	            	0.5,-0.5,1.0,0,
	            	0,0,0,0,
	            	0,0,0,1;
    log::log(INFO <<  "hello 4");
    log::log(INFO <<  sizeof(opengl::Sprite_2)<<"  sprite size");
    float x,y;
    int closed = 0;
    SDL_Event event;
	int frame = 0,j = 0,m=0;

    int tex_ids[40] = {2,5,12,689,695,716,779,795,859,855,849,351,343,342,330,339,320,326,354,361,357,363,499,576,578,581,584,591,594,601,600,805,61,64,67,71,171,179,181,186};
    int tree_list[6] = {1251,1254,1256,1258,1260,1262};
    for(int j = -6;j<4;j++){
        for(int z = 0;z<8;z++){
        terrains.push_back(new opengl::Sprite_2(250 + 512*z,250 + 512*j,512.0f,512.0f,(rand()%1000)/1000.0f,(rand()%1000)/1000.0f,(rand()%1000)/1000.0f,1.0f));
        //terrains.back()->set_terrain(6009 + (abs(j)*8 + z)%32);
        //terrains.push_back(new opengl::Sprite_2(250 + 512*z,250 + 512*j,512.0f,512.0f,(rand()%1000)/1000.0f,(rand()%1000)/1000.0f,(rand()%1000)/1000.0f,1.0f));
        //terrains.back()->set_terrain(6010);
    }
    }


    opengl::Sprite_2 test_terrain(600,0,512.0,512.0,0.0,0.0,0.0,1.0);


    for(int j = 0;j<20;j++){
        for(int z = 0;z<10;z++){
        trees.push_back(new opengl::Sprite_2((float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        trees.back()->set_texture(tree_list[rand()%6],true);
        trees.back()->set_subtex(0);
        }
    }

    for(int j = 0;j<100;j++){
        for(int z = 0;z<10;z++){
        sprites.push_back(new opengl::Sprite_2((float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        //sprites.back()->set_texture(tex_ids[rand()%40],true);
        sprites.back()->set_texture(795,true);
        }
    }

    /*for(int j = 0;j<10;j++){
        for(int z = 0;z<10;z++){
        sprites_2.push_back(new opengl::Sprite_2((float)(rand()%1920),(float)(rand()%1080),(float)100,(float)100,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)(rand()%1000)/1000.0,(float)1.0));
        sprites_2.back()->set_texture(tex_ids[10 + rand()%22],true);
        //sprites.back()->set_subtex(rand()%6);
        }
    }*/

    opengl::Sprite_2 archery(900.0f,100.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    archery.set_texture(21,true);
    opengl::Sprite_2 castle(100.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    castle.set_texture(305,false);
    opengl::Sprite_2 blacksmith(100.0f,700.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    blacksmith.set_texture(92,true);
    opengl::Sprite_2 barrack(200.0f,50.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    barrack.set_texture(145,true);
    opengl::Sprite_2 monastery(300.0f,700.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    monastery.set_texture(280,true);
    opengl::Sprite_2 market(500.0f,400.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    market.set_texture(818,true);
    opengl::Sprite_2 workshop(1000.0f,500.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    workshop.set_texture(956,true);
    opengl::Sprite_2 viking_ship(1000.0f,800.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    viking_ship.set_texture(699,true);
    opengl::Sprite_2 elephant(1800.0f,100.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    elephant.set_texture(805,true);
    opengl::Sprite_2 paladin(1800.0f,900.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    paladin.set_texture(673,true);
	opengl::Sprite_2 trade_cart(1800.0f,700.0f,200.0f,200.0f,1.0f,1.0f,0.0f,1.0f);
    trade_cart.set_texture(4486,true);

    std::sort(terrains.begin(),terrains.end(),compareSprite);
    std::sort(sprites.begin(),sprites.end(),compareSprite);
    std::sort(trees.begin(),trees.end(),compareSprite);
    shade->use();
    shade->texture_array();
    //shadebatch->use();

    /// texture array test
    auto test_data = resources::TextureData(path / "assets/terrain/textures/62.png",false);
    auto test_data_2 = resources::TextureData(path / "assets/terrain/textures/2.png",false);
    auto test_data_3 = resources::TextureData(path / "assets/terrain/textures/23.png",false);
    //auto tex_array = opengl::GlTextureArray(test_data);
    //auto new_array = opengl::GlTextureArray(3,512,512,resources::pixel_format::rgba8);
    auto terr_manager = opengl::TerrainManager(window.get_context(),path);
    terr_manager.init_terrain();
    //new_array.submit_texture(test_data);
    //new_array.submit_texture(test_data_2);
    //new_array.submit_texture(test_data_3);

    glActiveTexture(GL_TEXTURE0);
    terr_manager.bind();
    //new_array.bind();    
    //tex_array.bind();
    //shadebatch->sampler_array(0);
    /// end texture array test
    glDepthFunc(GL_LEQUAL); 
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // important to remove the black square around the textures or the transperent area.
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    int depth = 0;
       
    uint32_t time1,time2,time3;
    time1 = SDL_GetTicks();
    time2 = time1;
    time3 = time1;
    while(!closed){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		frame++;
        time1 = SDL_GetTicks();
		if(time1 - time2 >= 1000){
      		time2 = time1;
			log::log(INFO <<" frames " << frame);
			frame = 0;
            depth++;
		}

        if(time1 - time3 >= 50){ 
        //log::log(INFO << m);
        time3 = time1;
        viking_ship.set_subtex(m%9);
         elephant.set_subtex(0 + m%10);
        paladin.set_subtex(20 + m%10);
	    trade_cart.set_subtex(20 + m%10);
	    trade_cart.x -= 2.0;
        elephant.y += 1.5;
        paladin.x -= 2.0;
        m++;
        }
        terr_manager.render();
        /*auto batch_uniform = shadebatch->new_uniform_input("ortho",pers2,"layer",depth%32,"dimet",dimet);
        //auto batch_uniform = shadebatch->new_uniform_input("ortho",pers2,"layer",0);        
        auto lala_batch = dynamic_cast<opengl::GlUniformInput const*>(batch_uniform.get());
        shadebatch->use();
        shadebatch->execute_with(lala_batch,nullptr);        
        renderer->begin();
        for(int k=0;k<terrains.size();k++){
			//sprites[k]->x = rand()%1920;
			//sprites[k]->y = rand()%1080;
            renderer->submit(*(terrains[k]));
		}
        //renderer->submit(test_terrain);
        renderer->end();
        renderer->render();*/
        shade->use();
        auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"dimet",dimet);
        auto lala = dynamic_cast<opengl::GlUniformInput const*>(new_uniform.get());
        shade->execute_with(lala,nullptr);
        renderer_2->begin();
        for(int k=0;k<1000;k++){
			//sprites[k]->x = rand()%1920;
			//sprites[k]->y = rand()%1080;
            renderer_2->submit(*(sprites[k]));
		}
        /*for(int k=0;k<trees.size();k++){
			//sprites[k]->x = rand()%1920;
			//sprites[k]->y = rand()%1080;
            renderer_2->submit(*(trees[k]));
		}*/
        renderer_2->end();
        renderer_2->render();
        /*renderer_3->begin();
        renderer_3->submit(monastery);
        renderer_3->submit(archery);
        renderer_3->submit(castle);
        renderer_3->submit(barrack);
        renderer_3->submit(market);
        renderer_3->submit(elephant);
        renderer_3->submit(paladin);
        renderer_3->submit(workshop);
        renderer_3->submit(blacksmith);
        renderer_3->submit(trade_cart);
        renderer_3->end();
        renderer_3->render();*/

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