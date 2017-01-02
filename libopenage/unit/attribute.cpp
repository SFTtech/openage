// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#include "attribute.h"
#include "unit.h"
#include "unit_type.h"

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
		else if(unshared) {
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

bool Attribute<attr_type::dropsite>::accepting_resource(game_resource res) const {
	return std::find(resource_types.begin(), resource_types.end(), res) != resource_types.end();
}

void Attribute<attr_type::multitype>::switchType(const gamedata::unit_classes cls, Unit *unit) const {
	auto search = this->types.find(cls);
	if (search != this->types.end()) {
		auto &player = unit->get_attribute<attr_type::owner>();
		search->second->reinitialise(unit, player.player);
	}
}

} /* namespace openage */
