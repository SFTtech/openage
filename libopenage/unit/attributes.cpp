// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "attributes.h"

namespace openage {

void Attributes::add(const std::shared_ptr<AttributeContainer> attr) {
	this->attrs[attr->type] = attr;
}

void Attributes::add_copies(const Attributes &other) {
	this->add_copies(other, true, true);
}

void Attributes::add_copies(const Attributes &other, bool shared, bool unshared) {
	for (auto &i : other.attrs) {
		auto &attr = *i.second.get();

		if (attr.shared()) {
			if (shared) {
				// pass self
				this->add(i.second);
			}
		}
		else if (unshared) {
			// create copy
			this->add(attr.copy());
		}
	}
}

bool Attributes::remove(const attr_type type) {
	return this->attrs.erase(type) > 0;
}

bool Attributes::has(const attr_type type) const {
	return this->attrs.find(type) != this->attrs.end();
}

std::shared_ptr<AttributeContainer> Attributes::get(const attr_type type) const {
	return this->attrs.at(type);
}

template<attr_type T>
Attribute<T> &Attributes::get() const {
	return *reinterpret_cast<Attribute<T> *>(this->attrs.at(T).get());
}

} // namespace openage
