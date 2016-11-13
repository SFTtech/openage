// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <utility>

#include "../coord/phys3.h"
#include "../coord/phys3_serialization.h"
#include "../terrain/tile_range.h"
#include "../terrain/tile_range_serialization.h"
#include "attribute.h"
#include "attribute_watcher.h"

namespace openage {

template<typename T>
class Spied {
public:
	using type = T;

	Spied(T &value, std::function<void(T)> write_out)
		:
		value(value),
		write_out{write_out} {
	}

	Spied(T &value, curve::CurveRecord &watcher, id_t id, const char *name)
		:
		value(value),
		write_out{[&watcher, id, &name](T value) { watcher.write_out(id, value, name); }} {
	}

	operator T() const {
		return this->value;
	}

	Spied<T>& operator=(T value) {
		this->value = value;
		this->write_out(value);
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
	T &value;
	std::function<void(T)> write_out;
};

template<>
class Spied<coord::phys3_delta> {
public:
	Spied(coord::phys3_delta &value, curve::CurveRecord &watcher, id_t id, const char *name)
		:
		ne{value.ne, [&value, &watcher, id, name](coord::phys_t v) { watcher.write_out(id, coord::phys3_delta{v, value.se, value.up}, name); }},
		se{value.se, [&value, &watcher, id, name](coord::phys_t v) { watcher.write_out(id, coord::phys3_delta{value.ne, v, value.up}, name); }},
		up{value.up, [&value, &watcher, id, name](coord::phys_t v) { watcher.write_out(id, coord::phys3_delta{value.ne, value.se, v}, name); }},
		watcher{watcher},
		id{id},
		name{name} {
	}

	Spied<coord::phys3_delta>& operator=(coord::phys3_delta value) {
		this->ne = value.ne;
		this->se = value.se;
		this->up = value.up;
		return *this;
	}

	operator coord::phys3_delta() const {
		return coord::phys3_delta{this->ne, this->se, this->up};
	}

	Spied<coord::phys_t> ne;
	Spied<coord::phys_t> se;
	Spied<coord::phys_t> up;

private:
	curve::CurveRecord &watcher;
	id_t id;
	const char *name;
};

template<typename T>
auto operator*(const Spied<coord::phys3_delta>& op1, T op2) -> decltype(std::declval<coord::phys3_delta>() * op2)
{
	return op1 * op2;
}

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
		current{val_ref(attr), watcher, id, AttributeSpy::name},
		max{attr.max},
		hp_bar_height{attr.hp_bar_height} {
	}

	Spied<value_type> current;
	unsigned int &max;
	float &hp_bar_height;
};

template<>
class AttributeSpy<attr_type::direction> {
public:
	static constexpr const char *name = "dir";
	using type = AttributeSpy;
	using value_type = coord::phys3_delta;

	static value_type& val_ref(Attribute<attr_type::direction> &attr) {
		static_assert(std::is_same<value_type, decltype(attr.unit_dir)>::value, "spied type mismatch");
		return attr.unit_dir;
	}

	static AttributeSpy create(curve::CurveRecord &watcher, id_t id, Attribute<attr_type::direction> &attr) {
		return AttributeSpy{watcher, id, attr};
	}

	AttributeSpy(curve::CurveRecord &watcher, id_t id, Attribute<attr_type::direction> &attr)
		:
		unit_dir{val_ref(attr), watcher, id, AttributeSpy::name} {
	}

	Spied<value_type> unit_dir;
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
