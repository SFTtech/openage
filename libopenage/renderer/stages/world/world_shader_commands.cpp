// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "world_shader_commands.h"

#include <cstring>

#include "error/error.h"
#include "log/log.h"

namespace openage::renderer::world {

ShaderCommandTemplate::ShaderCommandTemplate(const std::string &template_code) :
	template_code{template_code} {}

bool ShaderCommandTemplate::load_commands(const util::Path &config_path) {
	try {
		log::log(INFO << "Loading shader commands config from: " << config_path);
		auto config_file = config_path.open();
		std::string line;
		std::stringstream ss(config_file.read());

		ShaderCommandConfig current_command;
		// if true, we are reading the code block for the current command.
		bool reading_code = false;
		std::string code_block;

		while (std::getline(ss, line)) {
			if (!line.empty() && line.back() == '\r') {
				line.pop_back();
			}
			// Trim whitespace from line
			line = trim(line);
			log::log(INFO << "Parsing line: " << line);

			// Skip empty lines and comments
			if (line.empty() || line[0] == '#') {
				continue;
			}

			if (reading_code) {
				if (line == "}") {
					reading_code = false;
					current_command.code = code_block;

					// Generate and add snippet
					std::string snippet = generate_snippet(current_command);
					add_snippet(current_command.placeholder_id, snippet);
					commands.push_back(current_command);

					// Reset for next command
					code_block.clear();
				}
				else {
					code_block += line + "\n";
				}
				continue;
			}

			if (line == "[COMMAND]") {
				current_command = ShaderCommandConfig{};
				continue;
			}

			// Parse key-value pairs
			size_t pos = line.find('=');
			if (pos != std::string::npos) {
				std::string key = trim(line.substr(0, pos));
				std::string value = trim(line.substr(pos + 1));

				if (key == "placeholder") {
					current_command.placeholder_id = value;
				}
				else if (key == "alpha") {
					uint8_t alpha = static_cast<uint8_t>(std::stoi(value));
					if (alpha % 2 == 0 && alpha >= 0 && alpha <= 254) {
						current_command.alpha = alpha;
					}
					else {
						log::log(ERR << "Invalid alpha value for command: " << alpha);
						return false;
					}
				}
				else if (key == "description") {
					current_command.description = value;
				}
				else if (key == "code") {
					if (value == "{") {
						reading_code = true;
						code_block.clear();
					}
				}
			}
		}

		return true;
	}
	catch (const std::exception &e) {
		log::log(ERR << "Failed to load shader commands: " << e.what());
		return false;
	}
}

bool ShaderCommandTemplate::add_snippet(const std::string &placeholder_id, const std::string &snippet) {
	if (snippet.empty()) {
		log::log(ERR << "Empty snippet for placeholder: " << placeholder_id);
		return false;
	}

	if (placeholder_id.empty()) {
		log::log(ERR << "Empty placeholder ID for snippet");
		return false;
	}

	// Check if the placeholder exists in the template
	std::string placeholder = "//@" + placeholder_id + "@";
	if (template_code.find(placeholder) == std::string::npos) {
		log::log(ERR << "Placeholder not found in template: " << placeholder_id);
		return false;
	}

	// Store the snippet
	snippets[placeholder_id].push_back(snippet);
	return true;
}

std::string ShaderCommandTemplate::generate_snippet(const ShaderCommandConfig &command) {
	return "case " + std::to_string(command.alpha) + ":\n"
	       + "\t\t// " + command.description + "\n"
	       + "\t\t" + command.code + "\t\tbreak;\n";
}

std::string ShaderCommandTemplate::generate_source() const {
	std::string result = template_code;

	// Process each placeholder
	for (const auto &[placeholder_id, snippet_list] : snippets) {
		std::string combined_snippets;

		// Combine all snippets for this placeholder
		for (const auto &snippet : snippet_list) {
			combined_snippets += snippet;
		}

		// Find and replace the placeholder
		std::string placeholder = "//@" + placeholder_id + "@";
		size_t pos = result.find(placeholder);
		if (pos == std::string::npos) {
			throw Error(MSG(err) << "Placeholder disappeared from template: " << placeholder_id);
		}

		// Replace placeholder with combined snippets
		result.replace(pos, placeholder.length(), combined_snippets);
	}

	return result;
}

std::string ShaderCommandTemplate::trim(const std::string &str) const {
	size_t first = str.find_first_not_of(" \t");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}
} // namespace openage::renderer::world
