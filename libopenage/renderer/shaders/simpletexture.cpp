// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "simpletexture.h"

SimpleTextureProgram::SimpleTextureProgram(RawProgram *prg)
	:
	Program{prg} {

	this->add_var(this->position);
	this->add_var(this->tex);
}
