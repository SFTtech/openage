// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "uniform_buffer.h"

#include "error/error.h"
#include "log/log.h"

#include "renderer/opengl/context.h"
#include "renderer/opengl/lookup.h"
#include "renderer/opengl/texture.h"
#include "renderer/opengl/uniform_input.h"
#include "renderer/opengl/util.h"


namespace openage::renderer::opengl {

GlUniformBuffer::GlUniformBuffer(const std::shared_ptr<GlContext> &context,
                                 size_t size,
                                 std::vector<GlInBlockUniform> uniforms,
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

	uniform_id_t unif_id = 0;
	for (auto &uniform : uniforms) {
		this->uniforms_by_name.insert(std::make_pair(uniform.name, unif_id));
		unif_id += 1;
	}

	glBindBufferRange(GL_UNIFORM_BUFFER, this->binding_point, *this->handle, 0, this->data_size);

	log::log(MSG(dbg) << "Created OpenGL uniform buffer (size: "
	                  << this->data_size << ", binding point: "
	                  << this->binding_point << ")");
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
	ENSURE(glunif_in->get_buffer().get() == this, "Uniform input passed to different buffer than it was created with.");

	this->bind();

	const auto &update_offs = glunif_in->update_offs;
	const auto &used_uniforms = glunif_in->used_uniforms;
	const auto &uniforms = this->uniforms;
	uint8_t const *data = glunif_in->update_data.data();

	size_t unif_count = used_uniforms.size();
	for (size_t i = 0; i < unif_count; ++i) {
		uniform_id_t unif_id = used_uniforms[i];
		auto offset = update_offs[unif_id];

		uint8_t const *ptr = data + offset.offset;
		auto &unif = uniforms[unif_id];
		auto loc = unif.offset;
		auto size = unif.size;

		glBufferSubData(GL_UNIFORM_BUFFER, loc, size, ptr);
	}
}

const std::vector<GlInBlockUniform> &GlUniformBuffer::get_uniforms() const {
	return this->uniforms;
}

bool GlUniformBuffer::has_uniform(const char *name) {
	return this->uniforms_by_name.contains(name);
}

void GlUniformBuffer::bind() const {
	glBindBuffer(GL_UNIFORM_BUFFER, *this->handle);
}

std::shared_ptr<UniformBufferInput> GlUniformBuffer::new_unif_in() {
	auto in = std::make_shared<GlUniformBufferInput>(this->shared_from_this());

	return in;
}

void GlUniformBuffer::set_unif(UniformBufferInput &in, const char *unif, void const *val, GLenum type) {
	auto &unif_in = dynamic_cast<GlUniformBufferInput &>(in);

	auto unif_id = this->uniforms_by_name.find(unif)->second;
	ENSURE(unif_id < this->uniforms.size(),
	       "Tried to set uniform with invalid ID " << unif_id);

	auto const &unif_data = this->uniforms[unif_id];

	ENSURE(type == unif_data.type,
	       "Tried to set uniform " << unif << " to a value of the wrong type.");

	size_t size = GL_UNIFORM_TYPE_SIZE.get(type);
	ENSURE(size == unif_data.size,
	       "Tried to set uniform " << unif << " to a value of the wrong size.");

	auto &update_off = unif_in.update_offs[unif_id];
	auto offset = update_off.offset;
	memcpy(unif_in.update_data.data() + offset, val, size);
	if (not update_off.used) [[unlikely]] { // only true if the uniform value was not set before
		auto lower_bound = std::lower_bound(
			std::begin(unif_in.used_uniforms),
			std::end(unif_in.used_uniforms),
			unif_id);
		unif_in.used_uniforms.insert(lower_bound, unif_id);
		update_off.used = true;
	}
}

void GlUniformBuffer::set_i32(UniformBufferInput &in, const char *unif, int32_t val) {
	this->set_unif(in, unif, &val, GL_INT);
}

void GlUniformBuffer::set_u32(UniformBufferInput &in, const char *unif, uint32_t val) {
	this->set_unif(in, unif, &val, GL_UNSIGNED_INT);
}

void GlUniformBuffer::set_f32(UniformBufferInput &in, const char *unif, float val) {
	this->set_unif(in, unif, &val, GL_FLOAT);
}

void GlUniformBuffer::set_f64(UniformBufferInput &in, const char *unif, double val) {
	this->set_unif(in, unif, &val, GL_DOUBLE);
}

void GlUniformBuffer::set_bool(UniformBufferInput &in, const char *unif, bool val) {
	this->set_unif(in, unif, &val, GL_BOOL);
}

void GlUniformBuffer::set_v2f32(UniformBufferInput &in, const char *unif, Eigen::Vector2f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_VEC2);
}

void GlUniformBuffer::set_v3f32(UniformBufferInput &in, const char *unif, Eigen::Vector3f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_VEC3);
}

void GlUniformBuffer::set_v4f32(UniformBufferInput &in, const char *unif, Eigen::Vector4f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_VEC4);
}

void GlUniformBuffer::set_v2i32(UniformBufferInput &in, const char *unif, Eigen::Vector2i const &val) {
	this->set_unif(in, unif, val.data(), GL_INT_VEC2);
}

void GlUniformBuffer::set_v3i32(UniformBufferInput &in, const char *unif, Eigen::Vector3i const &val) {
	this->set_unif(in, unif, val.data(), GL_INT_VEC3);
}

void GlUniformBuffer::set_v4i32(UniformBufferInput &in, const char *unif, Eigen::Vector4i const &val) {
	this->set_unif(in, unif, val.data(), GL_INT_VEC4);
}

void GlUniformBuffer::set_v2ui32(UniformBufferInput &in, const char *unif, Eigen::Vector2<uint32_t> const &val) {
	this->set_unif(in, unif, val.data(), GL_UNSIGNED_INT_VEC2);
}

void GlUniformBuffer::set_v3ui32(UniformBufferInput &in, const char *unif, Eigen::Vector3<uint32_t> const &val) {
	this->set_unif(in, unif, val.data(), GL_UNSIGNED_INT_VEC3);
}

void GlUniformBuffer::set_v4ui32(UniformBufferInput &in, const char *unif, Eigen::Vector4<uint32_t> const &val) {
	this->set_unif(in, unif, val.data(), GL_UNSIGNED_INT_VEC4);
}

void GlUniformBuffer::set_m4f32(UniformBufferInput &in, const char *unif, Eigen::Matrix4f const &val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_MAT4);
}

void GlUniformBuffer::set_tex(UniformBufferInput &in, const char *unif, std::shared_ptr<Texture2d> const &val) {
	auto tex = std::dynamic_pointer_cast<GlTexture2d>(val);
	GLuint handle = tex->get_handle();
	this->set_unif(in, unif, &handle, GL_SAMPLER_2D);
}


} // namespace openage::renderer::opengl
