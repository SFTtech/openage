// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_GEOMETRY_H_
#define OPENAGE_RENDERER_GEOMETRY_H_


namespace openage {
namespace renderer {

class Material;

class Geometry {
public:
	Geometry();
	virtual ~Geometry();

private:
	std::string name;

	Material *material;
};

}} // openage::renderer

#endif
