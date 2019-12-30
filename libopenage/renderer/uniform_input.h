// Copyright 2019-2019 the openage authors. See copying.md for legal info.


#pragma once

#include <memory>

#include <eigen3/Eigen/Dense>

#include "../error/error.h"


namespace openage::renderer {

class ShaderProgram;
class Texture2d;


/// The abstract base for uniform input. Besides the uniform values, it stores information about
/// which shader program the input was created for.
class UniformInput : public std::enable_shared_from_this<UniformInput> {
protected:
	UniformInput(std::shared_ptr<ShaderProgram> const&);
public:
	virtual ~UniformInput() = default;

	// Template dispatches for uniform variable setting.
	void update();
	void update(const char* unif, int32_t val);
	void update(const char* unif, uint32_t val);
	void update(const char* unif, float val);
	void update(const char* unif, double val);
	void update(const char* unif, Eigen::Vector2f const& val);
	void update(const char* unif, Eigen::Vector3f const& val);
	void update(const char* unif, Eigen::Vector4f const& val);
	void update(const char* unif, std::shared_ptr<Texture2d> const& val);
	void update(const char* unif, std::shared_ptr<Texture2d> & val);
	void update(const char* unif, Eigen::Matrix4f const& val);

	/* catch-all template in order to avoid infinite recursion */
	template<typename T>
	void update(const char* unif, T) {
		// TODO: maybe craft an static_assert that contains the `unif` content
		throw Error(MSG(err) << "Tried to set uniform '" << unif
		            << "' using unsupported type '" << util::typestring<T>() << "'");
	}

	/// Updates the given uniform input with new uniform values similarly to new_uniform_input.
	/// For example, update_uniform_input(in, "awesome", true) will set the "awesome" uniform
	/// in addition to whatever values were in the uniform input before.
	template<typename T, typename... Ts>
	void update(const char* unif, T val, Ts... vals) {
		this->update(unif, val);
		this->update(vals...);
	}

	std::shared_ptr<ShaderProgram> const& get_program() const {
		return this->program;
	}

protected:
	/// The program that this uniform input handle was created for.
	std::shared_ptr<ShaderProgram> program;
};


} // namespace openage::renderer
