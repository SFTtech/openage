// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "uniform_input.h"

#include "shader_program.h"


namespace openage::renderer {

UniformInput::UniformInput(std::shared_ptr<ShaderProgram> const &prog) :
	program{prog} {}

void UniformInput::update() {}

void UniformInput::update(const char *unif, int32_t val) {
	this->program->set_i32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, uint32_t val) {
	this->program->set_u32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, float val) {
	this->program->set_f32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, double val) {
	this->program->set_f64(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, bool val) {
	this->program->set_bool(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector2f const &val) {
	this->program->set_v2f32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector3f const &val) {
	this->program->set_v3f32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector4f const &val) {
	this->program->set_v4f32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector2i const &val) {
	this->program->set_v2i32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector3i const &val) {
	this->program->set_v3i32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector4i const &val) {
	this->program->set_v4i32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector2<uint32_t> const &val) {
	this->program->set_v2ui32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector3<uint32_t> const &val) {
	this->program->set_v3ui32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector4<uint32_t> const &val) {
	this->program->set_v4ui32(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, std::shared_ptr<Texture2d> const &val) {
	this->program->set_tex(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, std::shared_ptr<Texture2d> &val) {
	this->program->set_tex(this->shared_from_this(), unif, val);
}

void UniformInput::update(const char *unif, Eigen::Matrix4f const &val) {
	this->program->set_m4f32(this->shared_from_this(), unif, val);
}


} // namespace openage::renderer
