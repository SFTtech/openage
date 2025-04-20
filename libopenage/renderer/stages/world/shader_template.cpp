// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "shader_template.h"

#include <cstring>

#include "error/error.h"
#include "log/log.h"

namespace openage::renderer::world {

ShaderTemplate::ShaderTemplate(const util::Path &template_path) {
	auto file = template_path.open();
	this->template_code = file.read();
	file.close();
}

void ShaderTemplate::load_snippets(const util::Path &snippet_path) {
	// load config here
	util::Path snippet_path_copy = snippet_path;
	for (const auto &entry : snippet_path_copy.iterdir()) {
		if (entry.get_name().ends_with(".snippet")) {
			add_snippet(entry);
		}
	}
}

void ShaderTemplate::add_snippet(const util::Path &snippet_path) {
	auto file = snippet_path.open();
	std::string content = file.read();
	file.close();

	std::string name = snippet_path.get_stem();

	snippets[name] = content;
}

renderer::resources::ShaderSource ShaderTemplate::generate_source() const {
	std::string result_src = template_code;

	// Process each placeholder
	for (const auto &[name, snippet_code] : snippets) {
		std::string placeholder = "// PLACEHOLDER: " + name;
		size_t pos = result_src.find(placeholder);

		if (pos != std::string::npos) {
			result_src.replace(pos, placeholder.length(), snippet_code);
		}
		else {
			log::log(WARN << "Placeholder not found in template: " << name);
		}
	}

	// Check if all placeholders were replaced
	size_t placeholder_pos = result_src.find("// PLACEHOLDER:");
	if (placeholder_pos != std::string::npos) {
		size_t line_end = result_src.find('\n', placeholder_pos);
		std::string missing = result_src.substr(placeholder_pos,
		                                        line_end - placeholder_pos);
		throw Error(MSG(err) << "Missing snippet for placeholder: " << missing);
	}

	auto result = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		std::move(result_src));

	return result;
}

} // namespace openage::renderer::world
