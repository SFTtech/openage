// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "util/path.h"

namespace openage {
namespace renderer {
namespace world {

/**
 * Represents a single shader command that can be used in the world fragment shader.
 * Commands are identified by their alpha values and contain GLSL code snippets
 * that define custom rendering behavior.
 */
struct ShaderCommandConfig {
	/// ID of the placeholder where this snippet should be inserted
	std::string placeholder_id;
	/// Command identifier ((must be even, range 0-254))
	uint8_t alpha;
	/// GLSL code snippet that defines the command's behavior
	std::string code;
	/// Documentation (optional)
	std::string description;
};

/**
 * Manages shader templates and their code snippets.
 * Allows loading configurable shader commands and generating
 * complete shader source code.
 */
class ShaderCommandTemplate {
public:
	/**
	 * Create a shader template from source code of shader.
	 *
	 * @param template_code Source code containing placeholders.
	 */
	explicit ShaderCommandTemplate(const std::string &template_code);

	/**
	 * Load commands from a configuration file.
	 *
	 * @param config_path Path to the command configuration file.
	 * @return true if commands were loaded successfully.
	 */
	bool load_commands(const util::Path &config_path);

	/**
	 * Add a single code snippet to the template.
	 *
	 * @param placeholder_id Where to insert the snippet.
	 * @param snippet Code to insert.
	 * @return true if snippet was added successfully.
	 */
	bool add_snippet(const std::string &placeholder_id, const std::string &snippet);

	/**
	 * Generate final shader source code with all snippets inserted.
	 *
	 * @return Complete shader code.
	 * @throws Error if any required placeholders are missing snippets.
	 */
	std::string generate_source() const;

private:
	// Generate a single code snippet for a command.
	std::string generate_snippet(const ShaderCommandConfig &command);
	// Helper function to trim whitespace from a string
	std::string trim(const std::string &str) const;

	// Original template code with placeholders
	std::string template_code;
	// Mapping of placeholder IDs to their code snippets
	std::map<std::string, std::vector<std::string>> snippets;
	// Loaded command configurations
	std::vector<ShaderCommandConfig> commands;
};
} // namespace world
} // namespace renderer
} // namespace openage
