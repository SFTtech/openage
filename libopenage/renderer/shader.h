// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_SHADER_H_
#define OPENAGE_RENDERER_SHADER_H_


namespace openage {
namespace renderer {

enum class shadertype {
	fragment,
	vertex,
	geometry,
};


class Shader {
public:
	Shader(const std::string &path, shadertype type);
	~Shader();

private:
	std::string path;
	shadertype type;
};

}} // openage::renderer

#endif
