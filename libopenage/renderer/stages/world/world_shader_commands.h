// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <memory>
#include <vector>

namespace openage {
namespace renderer {
namespace world {

/**
 * Represents a single shader command that can be used in the world fragment shader.
 * Commands are identified by their alpha values and contain GLSL code snippets
 * that define custom rendering behavior.
 */
struct ShaderCommand {
	// Command identifier ((must be even, range 0-254))
	uint8_t alpha;
	// GLSL code snippet that defines the command's behavior
	std::string code;
	// Documentation (optional)
	std::string description;
};

/**
 * Manages shader commands for the world fragment shader.
 * Provides functionality to add, remove, and integrate commands into the base shader.
 * Commands are inserted at a predefined marker in the shader code.
 */
class WorldShaderCommands {
public:
	// Marker in shader code where commands will be inserted
	static constexpr const char *COMMAND_MARKER = "//@INSERT_COMMANDS@";

	/**
	 * Add a new shader command.
	 *
	 * @param alpha Command identifier (must be even, range 0-254)
	 * @param code GLSL code snippet defining the command's behavior
	 * @param description Human-readable description of the command's purpose
	 *
	 * @return true if command was added successfully, false if validation failed
	 */
	bool add_command(uint8_t alpha, const std::string &code, const std::string &description = "");

	/**
	 * Remove a command.
	 *
	 * @param alpha Command identifier (even values 0-254)
	 */
	bool remove_command(uint8_t alpha);

	/**
	 * Check if a command is registered.
	 *
	 * @param alpha Command identifier to check
	 *
	 * @return true if command is registered
	 */
	bool has_command(uint8_t alpha) const;

	/**
	 * Integrate registered commands into the base shader code.
	 *
	 * @param base_shader Original shader code containing the command marker
	 *
	 * @return Complete shader code with commands integrated at the marker position
	 *
	 * @throws Error if command marker is not found in the base shader
	 */
	std::string integrate_command(const std::string &base_shader);

private:
	/**
	 * Generate GLSL code for all registered commands.
	 *
	 * @return String containing case statements for each command
	 */
	std::string generate_command_code() const;

	/**
	 * Validate a command identifier.
	 *
	 * @param alpha Command identifier to validate
	 *
	 * @return true if alpha is even and within valid range (0-254)
	 */
	bool validate_alpha(uint8_t alpha) const;

	/**
	 * Validate command GLSL code.
	 *
	 * @param code GLSL code snippet to validate
	 *
	 * @return true if code is not empty (additional validation could be added)
	 */

	bool validate_code(const std::string &code) const;

	// Map of command identifiers to their respective commands
	std::map<uint8_t, ShaderCommand> commands_map;
};

} // namespace world
} // namespace renderer
} // namespace openage
