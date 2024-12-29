// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "world_shader_commands.h"

#include "error/error.h"
#include "log/log.h"

namespace openage::renderer::world {

bool WorldShaderCommands::add_command(uint8_t alpha, const std::string &code, const std::string &description) {
	if (!validate_alpha(alpha)) {
		log::log(ERR << "Invalid alpha value: " << int(alpha));
		return false;
	}
	if (!validate_code(code)) {
		log::log(ERR << "Invalid command code");
		return false;
	}

	commands_map[alpha] = {alpha, code, description};
	return true;
}

bool WorldShaderCommands::remove_command(uint8_t alpha) {
	if (!validate_alpha(alpha)) {
		return false;
	}
	commands_map.erase(alpha);
	return true;
}

bool WorldShaderCommands::has_command(uint8_t alpha) const {
	return commands_map.contains(alpha);
}

std::string WorldShaderCommands::integrate_command(const std::string &base_shader) {
	std::string final_shader = base_shader;
	std::string commands_code = generate_command_code();

	// Find the insertion point
	size_t insert_point = final_shader.find(COMMAND_MARKER);
	if (insert_point == std::string::npos) {
		throw Error(MSG(err) << "Failed to find command insertion point in shader.");
	}

	// Replace the insertion point with the generated command code
	final_shader.replace(insert_point, strlen(COMMAND_MARKER), commands_code);

	return final_shader;
}

std::string WorldShaderCommands::generate_command_code() const {
	std::string result = "";

	for (const auto &[alpha, command] : commands_map) {
		result += "    case " + std::to_string(alpha) + ":\n";
		result += "        // " + command.description + "\n";
		result += "        " + command.code + "\n";
		result += "        break;\n\n";
	}

	return result;
}

bool WorldShaderCommands::validate_alpha(uint8_t alpha) const {
	return alpha % 2 == 0 && alpha >= 0 && alpha <= 254;
}

bool WorldShaderCommands::validate_code(const std::string &code) const {
	return !code.empty();
}

} // namespace openage::renderer::world
