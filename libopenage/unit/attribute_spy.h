// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../terrain/tile_range.h"
#include "../terrain/tile_range_serialization.h"
#include "attribute.h"
#include "attribute_watcher.h"

namespace openage {

template<typename T>
class Spied {
public:
	using type = T;

	Spied(curve::CurveRecord &watcher, id_t id, T &value, const char *name)
		:
		watcher{watcher},
		id{id},
		value(value),
		name{name} {
	}

	operator T() const {
		return this->value;
	}

	Spied<T>& operator=(T value) {
		this->watcher.write_out(this->id, value, this->name);
		return *this;
	}

	template<typename U>
	Spied<T>& operator+=(U value) {
		return *this = *this + value;
	}

	template<typename U>
	Spied<T>& operator-=(U value) {
		return *this = *this - value;
	}

	template<typename U>
	Spied<T>& operator*=(U value) {
		return *this = *this * value;
	}

	template<typename U>
	Spied<T>& operator/=(U value) {
		return *this = *this / value;
	}

	template<typename U>
	Spied<T>& operator%=(U value) {
		return *this = *this % value;
	}

private:
	curve::CurveRecord &watcher;
	id_t id;
	T &value;
	const char *name;
};

template<attr_type T>
class AttributeSpy {
public:
	using type = Attribute<T>&;

	static Attribute<T>& create(curve::CurveRecord&, id_t, Attribute<T> &attr) {
		return attr;
	}
};

template<>
class AttributeSpy<attr_type::hitpoints> {
public:
	static constexpr const char *name = "hitpoints";
	using type = AttributeSpy;
	using value_type = unsigned int;

	static value_type& val_ref(Attribute<attr_type::hitpoints> &attr) {
		static_assert(std::is_same<value_type, decltype(attr.current)>::value, "spied type mismatch");
		return attr.current;
	}

	static AttributeSpy create(curve::CurveRecord &watcher, id_t id, Attribute<attr_type::hitpoints> &attr) {
		return AttributeSpy{watcher, id, attr};
	}

	AttributeSpy(curve::CurveRecord &watcher, id_t id, Attribute<attr_type::hitpoints> &attr)
		:
		current{watcher, id, val_ref(attr), AttributeSpy::name},
		max{attr.max},
		hp_bar_height{attr.hp_bar_height} {
	}

	Spied<value_type> current;
	unsigned int &max;
	float &hp_bar_height;
};

template<>
class AttributeSpy<attr_type::position> {
public:
	static constexpr const char *name = "pos";
	using value_type = tile_range;
};

/**
 * return attribute from a container while spying on it
 */
template<attr_type T>
typename AttributeSpy<T>::type get_attr(curve::CurveRecord &watcher, id_t id, attr_map_t &map) {
	return AttributeSpy<T>::create(watcher, id, get_attr_ref<T>(map));
}

/**
 * return attribute as constant from a container
 */
template<attr_type T>
const Attribute<T>& get_attr(const attr_map_t &map) {
	return get_attr_ref<T>(map);
}

template <attr_type T>
typename AttributeSpy<T>::value_type parse_attr(std::istream &stream) {
	typename AttributeSpy<T>::value_type val;
	stream >> val;
	return val;
}

} // namespace openage
