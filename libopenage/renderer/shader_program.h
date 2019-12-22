// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "uniform_input.h"
#include "resources/mesh_data.h"


namespace openage {
namespace renderer {

class Texture2d;

class ShaderProgram : public std::enable_shared_from_this<ShaderProgram> {
	friend UniformInput;

public:
	virtual ~ShaderProgram() = default;

	/// Returns whether the shader program contains a uniform variable with the given name.
	virtual bool has_uniform(const char* unif) = 0;

	/// Creates a new uniform input (a binding of uniform names to values) for this shader
	/// and optionally sets some uniform values. To do that, just pass two arguments -
	/// - a string literal and the value for that uniform for any uniform you want to set.
	/// For example new_uniform_input("color", { 0.5, 0.5, 0.5, 1.0 }, "num", 5) will set
	/// "color" to { 0.5, 0.5, 0.5, 0.5 } and "num" to 5. Types are important here and a type
	/// mismatch between the uniform variable and the input might result in an error.
	template<typename... Ts>
	std::shared_ptr<UniformInput> new_uniform_input(Ts... vals) {
		auto input = this->new_unif_in();
		input->update(vals...);
		return input;
	}

	/// Returns a list of _active_ vertex attributes in the shader program. Active attributes
	/// are those which have an effect on the shader output, meaning they are included in the
	/// output calculation and are not unused. Inactive attributes may or may not be present
	/// in the list - in particular, in the OpenGL implementation they will most likely be missing.
	/// The returned value is a map from the attribute location to its type. Locations need not
	/// be consecutive.
	virtual std::map<size_t, resources::vertex_input_t> vertex_attributes() const = 0;

protected:
	// Virtual dispatches to the actual shader program implementation.
	virtual std::shared_ptr<UniformInput> new_unif_in() = 0;
	virtual void set_i32(std::shared_ptr<UniformInput> const&, const char*, int32_t) = 0;
	virtual void set_u32(std::shared_ptr<UniformInput> const&, const char*, uint32_t) = 0;
	virtual void set_f32(std::shared_ptr<UniformInput> const&, const char*, float) = 0;
	virtual void set_f64(std::shared_ptr<UniformInput> const&, const char*, double) = 0;
	virtual void set_v2f32(std::shared_ptr<UniformInput> const&, const char*, Eigen::Vector2f const&) = 0;
	virtual void set_v3f32(std::shared_ptr<UniformInput> const&, const char*, Eigen::Vector3f const&) = 0;
	virtual void set_v4f32(std::shared_ptr<UniformInput> const&, const char*, Eigen::Vector4f const&) = 0;
	virtual void set_m4f32(std::shared_ptr<UniformInput> const&, const char*, Eigen::Matrix4f const&) = 0;
	virtual void set_tex(std::shared_ptr<UniformInput> const&, const char*, std::shared_ptr<Texture2d> const&) = 0;
};

}}
