// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <algorithm>

#include "attribute_spy.h"
#include "unit.h"

namespace openage {

using AttributeSetter = void(*)(std::istream&, id_t, UnitContainer&);

template<attr_type T>
void apply_to_attr(std::istream &stream, id_t id, UnitContainer &placed_units) {
	AttributeSpy<T>::val_ref(placed_units.get_unit(id).get()->get_attribute_unwatched<T>()) = parse_attr<T>(stream);
}

template<attr_type T>
struct ApplyToAttr {
	static constexpr std::tuple<const char*, AttributeSetter> named_setter = std::make_tuple(AttributeSpy<T>::name, apply_to_attr<T>);
};

template<>
void apply_to_attr<attr_type::position>(std::istream &stream, id_t id, UnitContainer &placed_units) {
	placed_units.get_unit(id).get()->location->pos = parse_attr<attr_type::position>(stream);
}

const std::tuple<const char*, AttributeSetter> parse_funcs[] = {
	ApplyToAttr<attr_type::direction>::named_setter,
	ApplyToAttr<attr_type::hitpoints>::named_setter,
	ApplyToAttr<attr_type::position>::named_setter,
};

void read_attr(std::istream &stream, id_t id, UnitContainer &placed_units) {
	const auto attr_name = parse_attr_name(stream);
	const auto foundIt = std::find_if(std::begin(parse_funcs), std::end(parse_funcs), [&attr_name](const std::tuple<const char*, AttributeSetter> &named_setter) {
		return std::get<const char*>(named_setter) == attr_name;
	});

	if (foundIt != std::end(parse_funcs))
		(std::get<AttributeSetter>(*foundIt))(stream, id, placed_units);
}

} // namespace openage
