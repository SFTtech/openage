// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "error/error.h"
#include "log/message.h"
#include "renderer/types.h"
#include "util/compiler.h"


namespace openage::renderer {
class ShaderProgram;
class Texture2d;
class UniformBuffer;

class DataInput {
public:
	virtual ~DataInput() = default;

	/**
	 * Template dispatches for uniform variable setting. Each method is used
	 * for a specific input type.
	 *
	 * @param unif ID of the uniform.
	 * @param val New uniform value.
	 */
	virtual void update() = 0;
	virtual void update(const char *unif, int32_t val) = 0;
	virtual void update(const char *unif, uint32_t val) = 0;
	virtual void update(const char *unif, float val) = 0;
	virtual void update(const char *unif, double val) = 0;
	virtual void update(const char *unif, bool val) = 0;
	virtual void update(const char *unif, Eigen::Vector2f const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector3f const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector4f const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector2i const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector3i const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector4i const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector2<uint32_t> const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector3<uint32_t> const &val) = 0;
	virtual void update(const char *unif, Eigen::Vector4<uint32_t> const &val) = 0;
	virtual void update(const char *unif, std::shared_ptr<Texture2d> const &val) = 0;
	virtual void update(const char *unif, std::shared_ptr<Texture2d> &val) = 0;
	virtual void update(const char *unif, Eigen::Matrix4f const &val) = 0;
};


/**
 * Abstract base for uniform input. Besides the uniform values, it stores information about
 * which shader program the input was created for.
 */
class UniformInput : public DataInput {
protected:
	/**
	 * Create a new uniform input for a given shader program.
	 *
	 * @param prog Shader program the uniform input belongs to.
	 */
	UniformInput(std::shared_ptr<ShaderProgram> const &prog);

public:
	virtual ~UniformInput() = default;

	virtual bool is_complete() const = 0;

	void update() override;
	void update(const char *unif, int32_t val) override;
	void update(const char *unif, uint32_t val) override;
	void update(const char *unif, float val) override;
	void update(const char *unif, double val) override;
	void update(const char *unif, bool val) override;
	void update(const char *unif, Eigen::Vector2f const &val) override;
	void update(const char *unif, Eigen::Vector3f const &val) override;
	void update(const char *unif, Eigen::Vector4f const &val) override;
	void update(const char *unif, Eigen::Vector2i const &val) override;
	void update(const char *unif, Eigen::Vector3i const &val) override;
	void update(const char *unif, Eigen::Vector4i const &val) override;
	void update(const char *unif, Eigen::Vector2<uint32_t> const &val) override;
	void update(const char *unif, Eigen::Vector3<uint32_t> const &val) override;
	void update(const char *unif, Eigen::Vector4<uint32_t> const &val) override;
	void update(const char *unif, std::shared_ptr<Texture2d> const &val) override;
	void update(const char *unif, std::shared_ptr<Texture2d> &val) override;
	void update(const char *unif, Eigen::Matrix4f const &val) override;

	void update(uniform_id_t id, int32_t val);
	void update(uniform_id_t id, uint32_t val);
	void update(uniform_id_t id, float val);
	void update(uniform_id_t id, double val);
	void update(uniform_id_t id, bool val);
	void update(uniform_id_t id, Eigen::Vector2f const &val);
	void update(uniform_id_t id, Eigen::Vector3f const &val);
	void update(uniform_id_t id, Eigen::Vector4f const &val);
	void update(uniform_id_t id, Eigen::Vector2i const &val);
	void update(uniform_id_t id, Eigen::Vector3i const &val);
	void update(uniform_id_t id, Eigen::Vector4i const &val);
	void update(uniform_id_t id, Eigen::Vector2<uint32_t> const &val);
	void update(uniform_id_t id, Eigen::Vector3<uint32_t> const &val);
	void update(uniform_id_t id, Eigen::Vector4<uint32_t> const &val);
	void update(uniform_id_t id, std::shared_ptr<Texture2d> const &val);
	void update(uniform_id_t id, std::shared_ptr<Texture2d> &val);
	void update(uniform_id_t id, Eigen::Matrix4f const &val);

	/**
	 * Catch-all template in order to handle unsupported types and avoid infinite recursion.
	 *
	 * @param unif ID of the uniform.
	 */
	template <typename T>
	void update(const char *unif, T) {
		// TODO: maybe craft an static_assert that contains the `unif` content
		throw Error(MSG(err) << "Tried to set uniform '" << unif
		                     << "' using unsupported type '" << util::typestring<T>() << "'");
	}

	/**
	 * Catch-all template in order to handle unsupported types and avoid infinite recursion.
	 *
	 * @param unif ID of the uniform.
	 */
	template <typename T>
	void update(const uniform_id_t &id, T) {
		// TODO: maybe craft an static_assert that contains the `unif` content
		throw Error(MSG(err) << "Tried to set uniform with ID " << id
		                     << " using unsupported type '" << util::typestring<T>() << "'");
	}

	/**
	 * Updates the given uniform input with new uniform values similarly to new_uniform_input.
	 * For example, update_uniform_input(in, "awesome", true) will set the "awesome" uniform
	 * in addition to whatever values were in the uniform input before.
	 *
	 * @param unif ID of the uniform.
	 */
	template <typename T, typename... Ts>
	void update(const char *unif, T val, Ts... vals) {
		this->update(unif, val);
		this->update(vals...);
	}

	/**
	 * Updates the given uniform input with new uniform values similarly to new_uniform_input.
	 * For example, update_uniform_input(in, "awesome", true) will set the "awesome" uniform
	 * in addition to whatever values were in the uniform input before.
	 *
	 * @param unif ID of the uniform.
	 */
	template <typename T, typename... Ts>
	void update(const uniform_id_t &id, T val, Ts... vals) {
		this->update(id, val);
		this->update(vals...);
	}

	/**
	 * Get the shader program the uniform input is used for.
	 *
	 * @return Shader program.
	 */
	std::shared_ptr<ShaderProgram> const &get_program() const {
		return this->program;
	}

protected:
	/**
	 * The program that this uniform input handle was created for.
	 */
	std::shared_ptr<ShaderProgram> program;
};


class UniformBufferInput : public DataInput
	, public std::enable_shared_from_this<UniformBufferInput> {
protected:
	UniformBufferInput(std::shared_ptr<UniformBuffer> const &buffer);

public:
	virtual ~UniformBufferInput() = default;

	void update() override;
	void update(const char *unif, int32_t val) override;
	void update(const char *unif, uint32_t val) override;
	void update(const char *unif, float val) override;
	void update(const char *unif, double val) override;
	void update(const char *unif, bool val) override;
	void update(const char *unif, Eigen::Vector2f const &val) override;
	void update(const char *unif, Eigen::Vector3f const &val) override;
	void update(const char *unif, Eigen::Vector4f const &val) override;
	void update(const char *unif, Eigen::Vector2i const &val) override;
	void update(const char *unif, Eigen::Vector3i const &val) override;
	void update(const char *unif, Eigen::Vector4i const &val) override;
	void update(const char *unif, Eigen::Vector2<uint32_t> const &val) override;
	void update(const char *unif, Eigen::Vector3<uint32_t> const &val) override;
	void update(const char *unif, Eigen::Vector4<uint32_t> const &val) override;
	void update(const char *unif, std::shared_ptr<Texture2d> const &val) override;
	void update(const char *unif, std::shared_ptr<Texture2d> &val) override;
	void update(const char *unif, Eigen::Matrix4f const &val) override;

	/**
	 * Catch-all template in order to handle unsupported types and avoid infinite recursion.
	 *
	 * @param unif ID of the uniform.
	 */
	template <typename T>
	void update(const char *unif, T) {
		// TODO: maybe craft an static_assert that contains the `unif` content
		throw Error(MSG(err) << "Tried to set uniform '" << unif
		                     << "' using unsupported type '" << util::typestring<T>() << "'");
	}

	/**
	 * Updates the given uniform input with new uniform values similarly to new_uniform_input.
	 * For example, update_uniform_input(in, "awesome", true) will set the "awesome" uniform
	 * in addition to whatever values were in the uniform input before.
	 *
	 * @param unif ID of the uniform.
	 */
	template <typename T, typename... Ts>
	void update(const char *unif, T val, Ts... vals) {
		this->update(unif, val);
		this->update(vals...);
	}

	/**
	 * Get the buffer the uniform input is used for.
	 *
	 * @return Uniform buffer.
	 */
	std::shared_ptr<UniformBuffer> const &get_buffer() const {
		return this->buffer;
	}

protected:
	/**
	 * The buffer that this uniform input handle was created for.
	 */
	std::shared_ptr<UniformBuffer> buffer;
};


} // namespace openage::renderer
