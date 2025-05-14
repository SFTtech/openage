// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "renderer/resources/shader_source.h"
#include "util/path.h"

namespace openage {
namespace renderer {
namespace resources {

/**
 * Manages shader templates and their code snippets.
 * Allows loading configurable shader commands and generating
 * complete shader source code.
 */
class ShaderTemplate {
public:
	/**
	 * Create a shader template from source code of shader.
	 *
	 * @param template_path Path to the template file.
	 */
	explicit ShaderTemplate(const util::Path &template_path);

	/**
	 * Load all snippets from a directory of .snippet files.
	 *
	 * @param snippet_path Path to directory containing snippet files.
	 */
	void load_snippets(const util::Path &snippet_path);

	/**
	 * Add a single code snippet to snippets map.
	 *
	 * @param name Snippet identifier.
	 * @param snippet_path Path to the snippet file.
	 */
	void add_snippet(const util::Path &snippet_path);

	/**
	 * Generate final shader source code with all snippets inserted.
	 *
	 * @return Complete shader code.
	 * @throws Error if any required placeholders are missing snippets.
	 */
	renderer::resources::ShaderSource generate_source() const;

private:
	/// Original template code with placeholders
	std::string template_code;
	/// Mapping of placeholder IDs to their code snippets
	std::map<std::string, std::string> snippets;

	/// Info about a placeholder found in the template
	struct Placeholder {
			std::string name;
			size_t position;
			size_t length;
	};

	/// All placeholders found in the template
	/// precomputed on creation
	std::vector<Placeholder> placeholders;
};
} // namespace world
} // namespace renderer
} // namespace openage
