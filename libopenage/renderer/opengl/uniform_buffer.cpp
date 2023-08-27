// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "uniform_buffer.h"

#include "error/error.h"
#include "renderer/opengl/context.h"
#include "renderer/opengl/lookup.h"
#include "renderer/opengl/texture.h"
#include "renderer/opengl/uniform_input.h"
#include "renderer/opengl/util.h"


namespace openage::renderer::opengl {

GlUniformBuffer::GlUniformBuffer(const std::shared_ptr<GlContext> &context,
                                 size_t size,
                                 std::unordered_map<std::string, GlInBlockUniform> uniforms,
                                 GLuint binding_point,
                                 GLenum usage) :
	GlSimpleObject(context,
                   [&](GLuint handle) {
					   glDeleteBuffers(1, &handle);
				   }),
	uniforms{uniforms},
	data_size{size},
	binding_point{binding_point} {
	GLuint handle;
	glGenBuffers(1, &handle);
	this->handle = handle;

	this->bind();
	glBufferData(GL_UNIFORM_BUFFER, this->data_size, NULL, usage);

	glBindBufferRange(GL_UNIFORM_BUFFER, this->binding_point, *this->handle, 0, this->data_size);
}

GLuint GlUniformBuffer::get_binding_point() const {
	return this->binding_point;
}

void GlUniformBuffer::set_binding_point(GLuint binding_point) {
	this->binding_point = binding_point;
	glBindBufferBase(GL_UNIFORM_BUFFER, this->binding_point, *this->handle);
}

void GlUniformBuffer::update_uniforms(std::shared_ptr<UniformBufferInput> const &unif_in) {
	auto glunif_in = std::dynamic_pointer_cast<GlUniformBufferInput>(unif_in);
	ENSURE(glunif_in->get_buffer() == this->shared_from_this(), "Uniform input passed to different buffer than it was created with.");

	this->bind();

	uint8_t const *data = glunif_in->update_data.data();
	for (auto const &pair : glunif_in->update_offs) {
		uint8_t const *ptr = data + pair.second;
		auto unif_def = this->uniforms[pair.first];
		auto loc = unif_def.offset;
		auto size = unif_def.size;

		glBufferSubData(GL_UNIFORM_BUFFER, loc, size, ptr);
	}
}

bool GlUniformBuffer::has_uniform(const char *unif) {
	return this->uniforms.count(unif) != 0;
}

void GlUniformBuffer::bind() const {
	glBindBuffer(GL_UNIFORM_BUFFER, *this->handle);
}

std::shared_ptr<UniformBufferInput> GlUniformBuffer::new_unif_in() {
	auto in = std::make_shared<GlUniformBufferInput>(this->shared_from_this());

	return in;
}

void GlUniformBuffer::set_unif(std::shared_ptr<UniformBufferInput> const &in, const char *unif, void const *val, GLenum type) {
	auto unif_in = std::dynamic_pointer_cast<GlUniformBufferInput>(in);

	auto uniform = this->uniforms.find(unif);
	ENSURE(uniform != std::end(this->uniforms),
	       "Tried to set uniform " << unif << " that does not exist in the shader program.");

	auto const &unif_data = uniform->second;

	ENSURE(type == unif_data.type,
	       "Tried to set uniform " << unif << " to a value of the wrong type.");

	size_t size = get_uniform_type_size(type);

	auto update_off = unif_in->update_offs.find(unif);
	if (update_off != std::end(unif_in->update_offs)) [[likely]] { // always used after the uniform value is written once
		// already wrote to this uniform since last upload
		size_t off = update_off->second;
		memcpy(unif_in->update_data.data() + off, val, size);
	}
	else {
		// first time writing to this uniform since last upload, so
		// extend the buffer before storing the uniform value
		size_t prev_size = unif_in->update_data.size();
		unif_in->update_data.resize(prev_size + size);
		memcpy(unif_in->update_data.data() + prev_size, val, size);
		unif_in->update_offs.emplace(unif, prev_size);
	}
}

void GlUniformBuffer::set_i32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, int32_t val) {
	this->set_unif(in, unif, &val, GL_INT);
}

void GlUniformBuffer::set_u32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, uint32_t val) {
	this->set_unif(in, unif, &val, GL_UNSIGNED_INT);
}

void GlUniformBuffer::set_f32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, float val) {
	this->set_unif(in, unif, &val, GL_FLOAT);
}

void GlUniformBuffer::set_f64(std::shared_ptr<UniformBufferInput> const &in, const char *unif, double val) {
	this->set_unif(in, unif, &val, GL_DOUBLE);
}

void GlUniformBuffer::set_bool(std::shared_ptr<UniformBufferInput> const &in, const char *unif, bool val) {
	this->set_unif(in, unif, &val, GL_BOOL);
}

void GlUniformBuffer::set_v2f32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector2f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_VEC2);
}

void GlUniformBuffer::set_v3f32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector3f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_VEC3);
}

void GlUniformBuffer::set_v4f32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector4f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_VEC4);
}

void GlUniformBuffer::set_v2i32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector2i const &val) {
	this->set_unif(in, unif, val.data(), GL_INT_VEC2);
}

void GlUniformBuffer::set_v3i32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector3i const &val) {
	this->set_unif(in, unif, val.data(), GL_INT_VEC3);
}

void GlUniformBuffer::set_v4i32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector4i const &val) {
	this->set_unif(in, unif, val.data(), GL_INT_VEC4);
}

void GlUniformBuffer::set_v2ui32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector2<uint32_t> const &val) {
	this->set_unif(in, unif, val.data(), GL_UNSIGNED_INT_VEC2);
}

void GlUniformBuffer::set_v3ui32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector3<uint32_t> const &val) {
	this->set_unif(in, unif, val.data(), GL_UNSIGNED_INT_VEC3);
}

void GlUniformBuffer::set_v4ui32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Vector4<uint32_t> const &val) {
	this->set_unif(in, unif, val.data(), GL_UNSIGNED_INT_VEC4);
}

void GlUniformBuffer::set_m4f32(std::shared_ptr<UniformBufferInput> const &in, const char *unif, Eigen::Matrix4f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_MAT4);
}

void GlUniformBuffer::set_tex(std::shared_ptr<UniformBufferInput> const &in, const char *unif, std::shared_ptr<Texture2d> const &val) {
	auto tex = std::dynamic_pointer_cast<GlTexture2d>(val);
	GLuint handle = tex->get_handle();
	this->set_unif(in, unif, &handle, GL_SAMPLER_2D);
}


} // namespace openage::renderer::opengl
