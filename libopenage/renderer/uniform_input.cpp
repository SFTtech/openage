// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#include "uniform_input.h"

#include "renderer/shader_program.h"
#include "renderer/uniform_buffer.h"


namespace openage::renderer {

UniformInput::UniformInput(std::shared_ptr<ShaderProgram> const &prog) :
	program{prog} {}

void UniformInput::update() {}

void UniformInput::update(const char *unif, int32_t val) {
	this->program->set_i32(*this, unif, val);
}

void UniformInput::update(const char *unif, uint32_t val) {
	this->program->set_u32(*this, unif, val);
}

void UniformInput::update(const char *unif, float val) {
	this->program->set_f32(*this, unif, val);
}

void UniformInput::update(const char *unif, double val) {
	this->program->set_f64(*this, unif, val);
}

void UniformInput::update(const char *unif, bool val) {
	this->program->set_bool(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector2f const &val) {
	this->program->set_v2f32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector3f const &val) {
	this->program->set_v3f32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector4f const &val) {
	this->program->set_v4f32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector2i const &val) {
	this->program->set_v2i32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector3i const &val) {
	this->program->set_v3i32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector4i const &val) {
	this->program->set_v4i32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector2<uint32_t> const &val) {
	this->program->set_v2ui32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector3<uint32_t> const &val) {
	this->program->set_v3ui32(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Vector4<uint32_t> const &val) {
	this->program->set_v4ui32(*this, unif, val);
}

void UniformInput::update(const char *unif, std::shared_ptr<Texture2d> const &val) {
	this->program->set_tex(*this, unif, val);
}

void UniformInput::update(const char *unif, std::shared_ptr<Texture2d> &val) {
	this->program->set_tex(*this, unif, val);
}

void UniformInput::update(const char *unif, Eigen::Matrix4f const &val) {
	this->program->set_m4f32(*this, unif, val);
}


void UniformInput::update(uniform_id_t id, int32_t val) {
	this->program->set_i32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, uint32_t val) {
	this->program->set_u32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, float val) {
	this->program->set_f32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, double val) {
	this->program->set_f64(*this, id, val);
}

void UniformInput::update(uniform_id_t id, bool val) {
	this->program->set_bool(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector2f const &val) {
	this->program->set_v2f32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector3f const &val) {
	this->program->set_v3f32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector4f const &val) {
	this->program->set_v4f32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector2i const &val) {
	this->program->set_v2i32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector3i const &val) {
	this->program->set_v3i32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector4i const &val) {
	this->program->set_v4i32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector2<uint32_t> const &val) {
	this->program->set_v2ui32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector3<uint32_t> const &val) {
	this->program->set_v3ui32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Vector4<uint32_t> const &val) {
	this->program->set_v4ui32(*this, id, val);
}

void UniformInput::update(uniform_id_t id, std::shared_ptr<Texture2d> const &val) {
	this->program->set_tex(*this, id, val);
}

void UniformInput::update(uniform_id_t id, std::shared_ptr<Texture2d> &val) {
	this->program->set_tex(*this, id, val);
}

void UniformInput::update(uniform_id_t id, Eigen::Matrix4f const &val) {
	this->program->set_m4f32(*this, id, val);
}


UniformBufferInput::UniformBufferInput(std::shared_ptr<UniformBuffer> const &buffer) :
	buffer{buffer} {}

void UniformBufferInput::update() {}

void UniformBufferInput::update(const char *unif, int32_t val) {
	this->buffer->set_i32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, uint32_t val) {
	this->buffer->set_u32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, float val) {
	this->buffer->set_f32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, double val) {
	this->buffer->set_f64(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, bool val) {
	this->buffer->set_bool(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector2f const &val) {
	this->buffer->set_v2f32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector3f const &val) {
	this->buffer->set_v3f32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector4f const &val) {
	this->buffer->set_v4f32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector2i const &val) {
	this->buffer->set_v2i32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector3i const &val) {
	this->buffer->set_v3i32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector4i const &val) {
	this->buffer->set_v4i32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector2<uint32_t> const &val) {
	this->buffer->set_v2ui32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector3<uint32_t> const &val) {
	this->buffer->set_v3ui32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Vector4<uint32_t> const &val) {
	this->buffer->set_v4ui32(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, std::shared_ptr<Texture2d> const &val) {
	this->buffer->set_tex(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, std::shared_ptr<Texture2d> &val) {
	this->buffer->set_tex(*this, unif, val);
}

void UniformBufferInput::update(const char *unif, Eigen::Matrix4f const &val) {
	this->buffer->set_m4f32(*this, unif, val);
}

} // namespace openage::renderer
