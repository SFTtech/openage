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
#include "RVO.h"
#include "RVOSimulator.h"
namespace openage {
namespace renderer {
namespace batch_test{

bool compareSprite(opengl::Sprite_2* i1,opengl::Sprite_2* i2)
{
    return (i1->tex_id < i2->tex_id);
}

std::vector<RVO::Vector2> goals;
std::vector<opengl::Sprite_2*> one;
std::vector<opengl::Sprite_2*> two;
std::vector<opengl::Sprite_2*> three;
std::vector<opengl::Sprite_2*> four;
std::vector<opengl::Sprite_2*> obstacles;

void setupScenario(RVO::RVOSimulator *sim)
{
#if RVO_SEED_RANDOM_NUMBER_GENERATOR
	std::srand(static_cast<unsigned int>(std::time(NULL)));
#endif

	/* Specify the global time step of the simulation. */
	sim->setTimeStep(0.25f);

	/* Specify the default parameters for agents that are subsequently added. */
	sim->setAgentDefaults(100.0f, 10, 5.0f, 5.0f, 10.0f, 2.0f);

	/*
	 * Add agents, specifying their start position, and store their goals on the
	 * opposite side of the environment.
	 */
	for (size_t i = 0; i < 5; ++i) {
		for (size_t j = 0; j < 5; ++j) {

			sim->addAgent(RVO::Vector2(200.0f + i * 10.0f,  200.0f + j * 10.0f));
            one.push_back(new opengl::Sprite_2(200.0f + i * 10.0f,200.0f + j * 10.0f,10,10,1.0,0.0,0.0,1.0));
			goals.push_back(RVO::Vector2(1500.0f, 900.0f));

			sim->addAgent(RVO::Vector2(200.0f - i * 10.0f,  900.0f + j * 10.0f));
            one.push_back(new opengl::Sprite_2(200.0f - i * 10.0f,900.0f + j * 10.0f,10,10,0.0,1.0,0.0,1.0));            
			goals.push_back(RVO::Vector2(1500.0f, 200.0f));

			sim->addAgent(RVO::Vector2(1500.0f + i * 10.0f, 900.0f - j * 10.0f));
            one.push_back(new opengl::Sprite_2(1500.0f + i * 10.0f, 900.0f - j * 10.0f,10,10,0.0,0.0,1.0,1.0));            
			goals.push_back(RVO::Vector2(200.0f, 200.0f));

			sim->addAgent(RVO::Vector2(1500.0f - i * 10.0f, 200.0f - j * 10.0f));
            one.push_back(new opengl::Sprite_2(1500.0f - i * 10.0f,200.0f - j * 10.0f,10,10,1.0,0.0,1.0,1.0));            
			goals.push_back(RVO::Vector2(200.0f, 900.0f));
		}
	}

	/*
	 * Add (polygonal) obstacles, specifying their vertices in counterclockwise
	 * order.
	 */
	/*std::vector<RVO::Vector2> obstacle1, obstacle2, obstacle3, obstacle4;
    float x,y,w,h;
    x = 250;
    y = 250;
    w = 20;
    h = 20;
	obstacle1.push_back(RVO::Vector2(x, y));
	obstacle1.push_back(RVO::Vector2(x, y+h));
	obstacle1.push_back(RVO::Vector2(x+w, y+h));
	obstacle1.push_back(RVO::Vector2(x+w, y));

	obstacle2.push_back(RVO::Vector2(10.0f, 40.0f));
	obstacle2.push_back(RVO::Vector2(10.0f, 10.0f));
	obstacle2.push_back(RVO::Vector2(40.0f, 10.0f));
	obstacle2.push_back(RVO::Vector2(40.0f, 40.0f));

	obstacle3.push_back(RVO::Vector2(10.0f, -40.0f));
	obstacle3.push_back(RVO::Vector2(40.0f, -40.0f));
	obstacle3.push_back(RVO::Vector2(40.0f, -10.0f));
	obstacle3.push_back(RVO::Vector2(10.0f, -10.0f));

	obstacle4.push_back(RVO::Vector2(-10.0f, -40.0f));
	obstacle4.push_back(RVO::Vector2(-10.0f, -10.0f));
	obstacle4.push_back(RVO::Vector2(-40.0f, -10.0f));
	obstacle4.push_back(RVO::Vector2(-40.0f, -40.0f));

	sim->addObstacle(obstacle1);
	sim->addObstacle(obstacle2);
	sim->addObstacle(obstacle3);
	sim->addObstacle(obstacle4);*/

	/* Process the obstacles so that they are accounted for in the simulation. */
	sim->processObstacles();
}

void updateVisualization(RVO::RVOSimulator *sim)
{
	/* Output the current global time. */
	std::cout << sim->getGlobalTime();

	/* Output the current position of all the agents. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		//log::log(INFO << " " << sim->getAgentPosition(i));
        RVO::Vector2 temp = sim->getAgentPosition(i);
        one[i]->x = temp.x();
        one[i]->y = temp.y();
	}

	std::cout << std::endl;
}

void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the goal.
	 */

	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i) {
		RVO::Vector2 goalVector = goals[i] - sim->getAgentPosition(i);

		if (RVO::absSq(goalVector) > 1.0f) {
			goalVector = RVO::normalize(goalVector);
		}

		sim->setAgentPrefVelocity(i, goalVector);

		/*
		 * Perturb a little to avoid deadlocks due to perfect symmetry.
		 */
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;

		sim->setAgentPrefVelocity(i, sim->getAgentPrefVelocity(i) +
		                          dist * RVO::Vector2(std::cos(angle), std::sin(angle)));
	}
}

bool reachedGoal(RVO::RVOSimulator *sim)
{
	/* Check if all agents have reached their goals. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		if (RVO::absSq(sim->getAgentPosition(i) - goals[i]) > 20.0f * 20.0f) {
			return false;
		}
	}

	return true;
}

void batch_demo(int demo_id,util::Path path){

    RVO::RVOSimulator *sim = new RVO::RVOSimulator();
    setupScenario(sim);
    opengl::GlWindow window("hello world",{1920, 1080});
    auto renderer = window.make_batchrenderer(path);
    auto renderer_2 = window.make_batchrenderer(path);
    auto renderer_3 = window.make_batchrenderer(path);
    auto renderer_4 = window.make_batchrenderer(path);
    auto renderer_5 = window.make_batchrenderer(path);
    
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
    clock_t curr_time,prev_time,update_time;
	int frame = 0,j = 0,m=0;
	prev_time = clock();
    update_time = clock();
    //auto test_texture = renderer->add_texture(tex);
    shade->texture_array();

    glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // important to remove the black square around the textures or the transperent area.
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    while(!closed and !reachedGoal(sim)){
        updateVisualization(sim);
        setPreferredVelocities(sim);
		sim->doStep();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		frame++;
		if((float)(clock()-prev_time)*4.0/CLOCKS_PER_SEC >= 1.0){
        prev_time = clock();
			log::log(INFO <<" frames " << frame);
			frame = 0;
		}
        auto new_uniform = shade->new_uniform_input("mouse_pos",Eigen::Vector2f(x,y),"ortho",pers2,"dimet",dimet);
        auto lala = dynamic_cast<opengl::GlUniformInput const*>(new_uniform.get());
        shade->execute_with(lala,nullptr);

        renderer->begin();
        for(int i = 0;i<one.size();i++){
            renderer->submit(*(one[i]));
        }
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