// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_SHADERS_SIMPLETEXTURE_H_
#define OPENAGE_RENDERER_SHADERS_SIMPLETEXTURE_H_

#include <memory>

#include "../pipeline.h"
#include "../../util/vector.h"

namespace openage {
namespace renderer {

class SimpleTexturePipeline : public Pipeline {
public:
	SimpleTexturePipeline(Program *prg);
	virtual ~SimpleTexturePipeline() = default;

	Uniform<Texture> tex;
	Attribute<util::Vector<4>, vertex_attribute_type::float_32, 4> position;
	Attribute<util::Vector<2>, vertex_attribute_type::float_32, 2> texcoord;
};


}} // openage::renderer


#endif
