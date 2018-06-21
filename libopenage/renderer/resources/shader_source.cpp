// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "shader_source.h"

#include "../../util/file.h"


namespace openage {
namespace renderer {
namespace resources {

ShaderSource::ShaderSource(shader_lang_t lang, shader_stage_t stage, std::string &&code)
	: lang(lang)
	, stage(stage)
	, code(std::move(code)) {}

ShaderSource::ShaderSource(shader_lang_t lang, shader_stage_t stage, const util::Path& path)
	: lang(lang)
	, stage(stage)
	, code(path.open().read()) {}

std::string const& ShaderSource::get_source() const {
	return this->code;
}

shader_lang_t ShaderSource::get_lang() const {
	return this->lang;
}

shader_stage_t ShaderSource::get_stage() const {
	return this->stage;
}

}}} // openage::renderer::resources
