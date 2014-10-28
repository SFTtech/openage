#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/glew.h>

#include <vector>

namespace openage {
namespace graphics {

struct vertex2 {
	float x;
	float y;
	
	static vertex2 Create (float _x, float _y)
	{
		vertex2 nVertex2;
		nVertex2.x = _x;
		nVertex2.y = _y;
		
		return nVertex2;
	}
};

	
struct rect {
	vertex2 topLeft;
	vertex2 bottomRight;
	
	static Create ( vertex2 _topLeft,
					vertex2 _bottomRight )
	{
		rect nRect;
		nRect.topLeft = _topLeft;
		nRect.bottomRight = _bottomRight;
		
		return nRect;
	}
};
	
struct renderQuad {
	rect pos;
	rect uv;
	rect maskUV;

	float zValue;
	unsigned playerID;

	
	static Create ( rect const & _pos,
					rect const & _uv,
					rect const & _maskUV,
				    float _zValue,
				    unsigned _playerID )
	{
		renderQuad nQuad;
		nQuad.pos = _pos;
		nQuad.uv = _uv;
		nQuad.maskUV = _maskUV;
		nQuad.zValue = _zValue;
		nQuad.playerID = _playerID;
		
		return nQuad;
	}
};

struct eMaterialType {
	enum Enum {
		keAlphaMask,
		keColorReplace,
		keNormal
	};
};
	
class Renderer {
public:
	void SubmitQuad (renderQuad const & quad,

					 GLint diffuse,
					 GLint mask,
					 
					 eMaterialType::Enum material) {}
	
private:
	typedef std::vector<renderQuad> renderQuadList;
	
	renderQuadList renderQueue;
};

}
}

#endif