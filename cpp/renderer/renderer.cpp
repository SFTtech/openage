#include "renderer.h"

namespace openage {
namespace graphics {
		
Renderer *Renderer::instance = NULL;
		
bool Renderer::create ()
{
	//Idempotent
	if ( instance != NULL ) return true;

	Renderer *nRenderer = new Renderer();
	
	if ( nRenderer->init() )
	{
		instance = nRenderer;
		return true;
	}
	
	delete nRenderer;
	return false;
}
		
Renderer &Renderer::get()
{
	//TODO: Should assert here if instance == NULL
	
	return *instance;
}
		
		
Renderer::Renderer ()
{
	//Constructor
}
		
bool Renderer::init ()
{
	return true;
}
		
void Renderer::submit_quad (renderQuad const & quad,
				 GLint diffuse,
				 GLint mask,
				 eMaterialType::Enum material)
{
	//Stuff data into vertex buffer
	
	//Generate a renderer key from
	//	MaterialType(16bit) | {textureHandle(16bit) | maskHandle(16bit)} | QuadIdx (16bit)
}

void Renderer::render ()
{
	//Sort the renderlist
	//Build the index buffer (dependent on sorting order of quads)
	//	Build a vector of *boundries* used when rendering
	
	
	//Bind vertex buffer
	//Bind index buffer
	
	
	//Clear the depth buffer
	//Enable depth test
	//Enable depth write
	
	
	//Foreach materialtype
	//	bind shader
	
	//	Foreach item in boundries
	//		drawelements using boundry information
}

}
}