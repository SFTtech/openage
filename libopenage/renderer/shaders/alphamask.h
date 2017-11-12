// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_SHADERS_ALPHAMASK_H_
#define OPENAGE_RENDERER_SHADERS_ALPHAMASK_H_

#include <memory>

#include "../material.h"
#include "../../util/vector.h"

namespace openage {
namespace renderer {

/**
 * A simple plain texture material.
 */
class AlphamaskMaterial : public Material {
public:
	AlphamaskMaterial(Program *prg);
	virtual ~AlphamaskMaterial() = default;

	void set_positions(mesh_t positions) override;

	// shader variables:
	Attribute<position_t, 4> position;
	Uniform<Texture *> base_texture;
	Uniform<Texture *> mask_texture;

	Uniform<bool> show_mask;
	Attribute<util::Vector<2>, 2> base_tex_coordinates;
	Attribute<util::Vector<2>, 2> mask_tex_coordinates;
};


}} // openage::renderer


#endif
