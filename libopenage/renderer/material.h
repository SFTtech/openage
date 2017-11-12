// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_MATERIAL_H_
#define OPENAGE_RENDERER_MATERIAL_H_

#include <memory>

#include "texture.h"
#include "shader.h"

namespace openage {
namespace renderer {

class Material {
protected:
	std::shared_ptr<Texture> txt;
	std::shared_ptr<RawProgram> code;
};

}} // namespace openage::renderer

#endif
