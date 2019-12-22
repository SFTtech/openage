// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <optional>

#include <epoxy/gl.h>


namespace openage {
namespace renderer {
namespace opengl {

class GlContext;


/// A base class for all classes representing OpenGL Objects to inherit from.
/// It allows moving the object, but not copying it through the copy constructor.
/// It has unique_ptr-like semantics. It is called 'simple', because in the future
/// we might want to add collections of objects and similar more advanced features.
class GlSimpleObject {
public:
	// Moving the representation is okay.
	GlSimpleObject(GlSimpleObject&&);
	GlSimpleObject &operator =(GlSimpleObject&&);

	// Generally, copying GL objects is costly and if we want to allow it,
	// we do so through an explicit copy() function.
	GlSimpleObject(GlSimpleObject const&) = delete;
	GlSimpleObject &operator =(GlSimpleObject const&) = delete;

	/// Uses delete_fun to destroy the underlying object,
	/// but only if the handle is present (hasn't been moved out).
	virtual ~GlSimpleObject();

	/// Returns the handle to the underlying OpenGL Object.
	GLuint get_handle() const;

protected:
	explicit GlSimpleObject(const std::shared_ptr<GlContext> &,
	                        std::function<void(GLuint)> &&delete_fun);

	/// Context, in which this object is valid in.
	/// We hold this reference to keep the context active as long as this object
	/// is not deconstructed.
	std::shared_ptr<GlContext> context;

	/// The handle to the OpenGL Object that this class represents.
	std::optional<GLuint> handle;

	/// The function that deletes the underlying OpenGL Object.
	std::function<void(GLuint)> delete_fun;
};

}}} // openage::renderer::opengl
