// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "shader_template.h"

#include <cstring>

#include "error/error.h"
#include "log/log.h"

namespace openage::renderer::resources {

ShaderTemplate::ShaderTemplate(const util::Path &template_path) {
	auto file = template_path.open();
	this->template_code = file.read();
	file.close();

	std::string marker = "// PLACEHOLDER: ";
	size_t pos = 0;

	while ((pos = this->template_code.find(marker, pos)) != std::string::npos) {
		size_t name_start = pos + marker.length();
		size_t line_end = this->template_code.find('\n', name_start);
		std::string name = this->template_code.substr(name_start, line_end - name_start);
		// Trim trailing whitespace (space, tab, carriage return, etc.)
		name.erase(name.find_last_not_of(" \t\r\n") + 1);

		this->placeholders.push_back({name, pos, line_end - pos});
		pos = line_end;
	}
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

	// Replace placeholders in reverse order (to avoid offset issues)
	for (auto it = placeholders.rbegin(); it != placeholders.rend(); ++it) {
		const auto &ph = *it;
		auto snippet_it = snippets.find(ph.name);
		if (snippet_it != snippets.end()) {
			result_src.replace(ph.position, ph.length, snippet_it->second);
		}
		else {
			throw Error(MSG(err) << "Missing snippet for placeholder: " << ph.name);
		}
	}

	auto result = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		std::move(result_src));

	return result;
}

} // namespace openage::renderer::world
