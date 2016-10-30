// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "attribute.h"
#include "attribute_watcher.h"

namespace openage {

template<typename T> class Spied {
public:
	Spied(AttributeWatcher &watcher, id_t id, T &value, const char *name)
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
		this->watcher.apply(this->id, value, this->name);
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
	AttributeWatcher &watcher;
	id_t id;
	T &value;
	const char *name;
};

template<attr_type T> class AttributeSpy {
public:
	using type = Attribute<T>&;

	static Attribute<T>& create(AttributeWatcher&, id_t, Attribute<T> &attr) {
		return attr;
	}
};

template<> class AttributeSpy<attr_type::hitpoints> {
public:
	using type = AttributeSpy<attr_type::hitpoints>;

	static AttributeSpy<attr_type::hitpoints> create(AttributeWatcher &watcher, id_t id, Attribute<attr_type::hitpoints> &attr) {
		return AttributeSpy<attr_type::hitpoints>{watcher, id, attr};
	}

	AttributeSpy(AttributeWatcher &watcher, id_t id, Attribute<attr_type::hitpoints> &attr)
		:
		current{watcher, id, attr.current, "hitpoints"},
		max{attr.max},
		hp_bar_height{attr.hp_bar_height} {
	}

	Spied<unsigned int> current;
	unsigned int &max;
	float &hp_bar_height;
};

/**
 * return attribute from a container while spying on it
 */
template<attr_type T> typename AttributeSpy<T>::type get_attr(AttributeWatcher &watcher, id_t id, attr_map_t &map) {
	return AttributeSpy<T>::create(watcher, id, get_attr_ref<T>(map));
}

/**
 * return attribute as constant from a container
 */
template<attr_type T> const Attribute<T>& get_attr(const attr_map_t &map) {
	return get_attr_ref<T>(map);
}

} // namespace openage
