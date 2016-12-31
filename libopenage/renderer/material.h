// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_MATERIAL_H_
#define OPENAGE_RENDERER_MATERIAL_H_

#include <vector>

#include "pipeline.h"
#include "../util/vector.h"

namespace openage {
namespace renderer {

/**
 * A material is a pipeline configuration that allows to setting predefined
 * properties.
 */
class Material : public Pipeline {
public:
	// standard position point position.
	using position_t = util::Vector<4>;
	using mesh_t = std::vector<position_t>;

	/**
	 * Creates a "type wrapper" around the pipeline program.
	 * This "Material" is then used to create pipeline
	 * properties and rendering algorithms.
	 */
	Material(Program *prg);
	virtual ~Material() = default;

	virtual void set_positions(mesh_t positions) = 0;
};

}} // openage::renderer

#endif
