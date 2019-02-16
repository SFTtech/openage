// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "../resources/shader_source.h"
#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// A single OpenGL shader stage.
class GlShader final : public GlSimpleObject {
public:
	/// Compiles the shader from the given source.
	explicit GlShader(const std::shared_ptr<GlContext> &context,
	                  const resources::ShaderSource&);

	/// Returns the stage of the rendering pipeline this shader defines.
	GLenum get_type() const;

private:
	/// Which stage of the rendering pipeline this shader defines.
	GLenum type;
};

}}} // openage::renderer::opengl
