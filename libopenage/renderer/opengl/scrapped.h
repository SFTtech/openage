/*
THIS IS AN EXTENSION TO UNIFORM PARSING
ALSO PARSE VERTEX ATTRIBUTES AND PROVIDE INFORMATION ABOUT THEM

GLint Program::get_uniformbuffer_id(const char *name) {
	this->check_is_linked("Uniform buffer requested");
	return glGetUniformBlockIndex(this->id, name);
}

/// Return the opengl layout id for a given vertex attribute name.
GLint Program::get_attribute_id(const char *name) {
	this->check_is_linked("Vertex attribute requested");

	GLint aid = glGetAttribLocation(this->id, name);

	if (unlikely(aid == -1)) {
		this->dump_attributes();
		throw Error{MSG(err) << "Attribute " << name
		                     << " queried but not found or active"
		                     << " (optimized out by the compiler?).", true};
	}

	return aid;
}

/// Set vertex attribute with given name to have a custom id.
void Program::set_attribute_id(const char *name, GLuint id) {
	if (unlikely(this->is_linked)) {
		throw Error{MSG(err)
			<< "you assigned attribute '" << name << " = "
			<< id << "' after program was linked!", true};
	}
	else {
		glBindAttribLocation(this->id, id, name);
	}
}

/// Query OpenGL which of the vertex attributes are actually active
/// and haven't been optimized out by the compiler.
void Program::dump_attributes() {
	auto msg = MSG(info);
	msg << "Dumping shader program " << this->id << " active attribute list:";

	GLint num_attribs;
	glGetProgramiv(this->id, GL_ACTIVE_ATTRIBUTES, &num_attribs);

	GLint attrib_max_length;
	glGetProgramiv(this->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrib_max_length);

	for (int i = 0; i < num_attribs; i++) {
		GLsizei attrib_length;
		GLint attrib_size;
		GLenum attrib_type;
		auto attrib_name = std::make_unique<char[]>(attrib_max_length);

		glGetActiveAttrib(this->id, i, attrib_max_length, &attrib_length, &attrib_size, &attrib_type, attrib_name.get());

		msg << "\n -> attribute " << attrib_name
		    << ": type=" << attrib_type << ", size=" << attrib_size
		    << ", id=" << this->get_attribute_id(attrib_name.get());
	}

	log::log(msg);
}
*/
