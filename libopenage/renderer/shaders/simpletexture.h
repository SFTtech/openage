// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_SHADERS_SIMPLETEXTURE_H_
#define OPENAGE_RENDERER_SHADERS_SIMPLETEXTURE_H_

#include <memory>

#include "../material.h"
#include "../../util/vector.h"

namespace openage {
namespace renderer {

/**
 * A simple plain texture material.
 */
class SimpleTextureMaterial : public Material {
public:
	SimpleTextureMaterial(Program *prg);
	virtual ~SimpleTextureMaterial() = default;

	void set_positions(mesh_t positions) override;

	// shader variables:
	Uniform<Texture> tex;
	Attribute<position_t, vertex_attribute_type::float_32, 4> position;
	Attribute<util::Vector<2>, vertex_attribute_type::float_32, 2> texcoord;
};


}} // openage::renderer


#endif
