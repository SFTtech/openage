// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <map>

#include "attribute.h"

namespace openage {

/**
 * Contains a group of attributes.
 * Can contain only one attribute of each type.
 */
class Attributes {
public:
	Attributes() {}

	/**
	 * Add an attribute or replace any attribute of the same type.
	 */
	void add(const std::shared_ptr<AttributeContainer> attr);

	/**
	 * Add copies of all the attributes from the given Attributes.
	 */
	void add_copies(const Attributes &attrs);

	/**
	 * Add copies of all the attributes from the given Attributes.
	 * If shared is false, shared attributes are ignored.
	 * If unshared is false, unshared attributes are ignored.
	 */
	void add_copies(const Attributes &attrs, bool shared, bool unshared);

	/**
	 * Remove an attribute based on the type.
	 */
	bool remove(const attr_type type);

	/**
	 * Check if the attribute of the given type exists.
	 */
	bool has(const attr_type type) const;

	/**
	 * Get the attribute based on the type.
	 */
	std::shared_ptr<AttributeContainer> get(const attr_type type) const;

	/**
	 * Get the attribute
	 */
	template<attr_type T>
	Attribute<T> &get() const {
		return *reinterpret_cast<Attribute<T> *>(this->attrs.at(T).get());
	}

private:

	std::map<attr_type, std::shared_ptr<AttributeContainer>> attrs;
};

} // namespace openage
