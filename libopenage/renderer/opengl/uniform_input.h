// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "../renderer.h"


namespace openage {
namespace renderer {
namespace opengl {

class GlShaderProgram;

/// Describes OpenGL-specific uniform valuations.
struct GlUniformInput : public UniformInput {
	/// The program that this was created for.
	GlShaderProgram* program;

	/// We store uniform updates lazily. They are only actually uploaded to GPU
	/// when a draw call is made. Update_offs maps the uniform names to where their
	/// value is in update_data in terms of a byte-wise offset. This is only a partial
	/// valuation, so not all uniforms have to be present here.
	std::unordered_map<std::string, size_t> update_offs;

	/// Buffer containing untyped uniform update data.
	std::vector<uint8_t> update_data;
};

}}}
