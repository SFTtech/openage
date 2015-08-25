// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_SHADERS_SIMPLETEXTURE_H_
#define OPENAGE_RENDERER_SHADERS_SIMPLETEXTURE_H_

#include <memory>

#include "../shader.h"


class SimpleTextureProgram : public Program {
public:
	SimpleTextureProgram(RawProgram *prg);
	virtual ~SimpleTextureProgram();

	Uniform<vec4> color;
	Attribute<vec2> position;
	Attribute<vec3> color_tint;
};


void lol() {
	SimpleTextureProgram asdf{program};

	asdf.color = {1, 0, 0, 1};
	asdf.tex = tentacle_monster;

	DrawHandle draw = asdf.get_handle();
	draw.position = {
		{0, 0},
		{0, 1},
		{1, 0},
		{1, 1}
	};

	draw.color_tint = {
		{0, 0, 0},
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
	};

	// enqueue the call, this tests the constraints internally.
	// len(whatever)/pervertex == len(positions)/pervertex
	// packs the buffers when needed,
	// submits to gpu when needed.
	renderer.add_task(draw.get_task());
}


#endif
