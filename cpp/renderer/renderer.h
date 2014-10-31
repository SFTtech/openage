#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/glew.h>

#include <vector>


namespace openage {

namespace shader {
	class Program;
}
namespace util {
	class Dir;
}
	
namespace graphics {

struct vertex2 {
	float x;
	float y;
	
	static vertex2 Create (float _x, float _y) {
		vertex2 nVertex2;
		nVertex2.x = _x;
		nVertex2.y = _y;
		
		return nVertex2;
	}
};

	
struct rect {
	vertex2 topLeft;
	vertex2 bottomRight;
	
	static rect Create( vertex2 _topLeft,
	                     vertex2 _bottomRight ) {
		rect nRect;
		nRect.topLeft = _topLeft;
		nRect.bottomRight = _bottomRight;
		
		return nRect;
	}
};
	
struct render_quad {
	struct quad_vertex {
		vertex2 pos;
		vertex2 uv;
		vertex2 maskUV;
		float zValue;
		unsigned playerID;
	};
	
	quad_vertex vertices[4];

	static render_quad Create ( rect const & _pos,
	                            rect const & _uv,
	                            rect const & _maskUV,
	                            float _zValue,
	                            unsigned _playerID ) {
		render_quad nQuad;

		nQuad.vertices[0].pos      = _pos.topLeft;
		nQuad.vertices[0].uv       = _uv.topLeft;
		nQuad.vertices[0].maskUV   = _maskUV.topLeft;
		nQuad.vertices[0].zValue   = _zValue;
		nQuad.vertices[0].playerID = _playerID;

		nQuad.vertices[1].pos      = vertex2::Create(_pos.topLeft.x, _pos.bottomRight.y);
		nQuad.vertices[1].uv       = vertex2::Create(_uv.topLeft.x, _uv.bottomRight.y);
		nQuad.vertices[1].maskUV   = vertex2::Create(_maskUV.topLeft.x, _maskUV.bottomRight.y);
		nQuad.vertices[1].zValue   = _zValue;
		nQuad.vertices[1].playerID = _playerID;
		
		nQuad.vertices[2].pos      = _pos.bottomRight;
		nQuad.vertices[2].uv       = _uv.bottomRight;
		nQuad.vertices[2].maskUV   = _maskUV.bottomRight;
		nQuad.vertices[2].zValue   = _zValue;
		nQuad.vertices[2].playerID = _playerID;
		
		nQuad.vertices[3].pos      = vertex2::Create(_pos.bottomRight.x, _pos.topLeft.y);
		nQuad.vertices[3].uv       = vertex2::Create(_uv.bottomRight.x, _uv.topLeft.y);
		nQuad.vertices[3].maskUV   = vertex2::Create(_maskUV.bottomRight.x, _maskUV.topLeft.y);
		nQuad.vertices[3].zValue   = _zValue;
		nQuad.vertices[3].playerID = _playerID;
		
		return nQuad;
	}
};

struct eMaterialType {
	enum Enum {
		keNormal,
		keColorReplace,
		keAlphaMask,
		
		keCount
	};
};
	
struct material {
	shader::Program *program;
	GLint uniformNormalTexture;
	GLint uniformMasktexture;
	GLint uniformPlayerColors;
	
	GLint attributeUV;
	GLint attributeMaskUV;
	
	GLint attributePlayerCol;
	GLint attributeZOrder;
};
	

	
class Renderer {
public:
	void submit_quad (render_quad const & quad,
	                  GLint diffuse,
	                  GLint mask,
					  unsigned char layer,
	                  eMaterialType::Enum material_type);
	
	void render ();
	
	static bool create (util::Dir const *data_dir, util::Dir const *asset_dir);
	static Renderer &get();
	
private:

	Renderer ();
	bool init (util::Dir const *data_dir, util::Dir const *asset_dir);
	
	void apply_material (eMaterialType::Enum material_type);
	void disable_material (eMaterialType::Enum material_type);
	
	Renderer(const Renderer &copy) = delete;
	Renderer &operator=(const Renderer &copy) = delete;
	Renderer(Renderer &&other) = delete;
	Renderer &operator=(Renderer &&other) = delete;
	
private:
	struct render_token_struct {
		short idx;
		short mask;
		short diffuse;
		unsigned char matType;
		unsigned char layer;
	} ;
	
	typedef unsigned long long render_token;
	
	typedef std::vector<render_quad> render_quad_list;
	typedef std::vector<render_token> render_queue_list;
	render_quad_list render_buffer;
	render_queue_list render_queue;
	
	material materials[eMaterialType::keCount];
	
	static Renderer *instance;
};

}
}

#endif