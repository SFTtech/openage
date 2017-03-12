// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "../renderer.h"


namespace openage {
namespace renderer {
namespace opengl {

class GlShaderProgram;

struct GlUniformInput : public UniformInput {
	GlShaderProgram* program;
	// store uniforms updates lazily
	// execute_with reads, uploads, and clears
	// std::unordered_map<std::string, UniformValueUpdate> updates;
	// enable partial valuations - only some uniforms

	/// Map from uniform names to their locations in the update buffer.
	std::unordered_map<std::string, size_t> update_offs;

	/// Buffer containing untyped uniform update data.
	std::vector<uint8_t> update_data;
};

}}}
