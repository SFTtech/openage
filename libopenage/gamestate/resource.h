// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

namespace openage {

/**
 * probably becomes part of the nyan game spec in future
 */
enum class game_resource {
	wood,
	food,
	gold,
	stone,
	RESOURCE_TYPE_COUNT
};

class ResourceBoundle {
public:

	ResourceBoundle();

	float& operator[] (game_resource res) { return value[(int) res]; }
	float& operator[] (int index) { return value[index]; }

	float get(game_resource res) const { return value[(int) res]; }
	float get(int index) const { return value[index]; }

	bool operator> (const ResourceBoundle& other) const {
		for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
			if (!(this->get(i) > other.get(i))) {
				return false;
			}
		}
		return true;
	}
	bool operator>= (const ResourceBoundle& other) const {
		for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
			if (!(this->get(i) >= other.get(i))) {
				return false;
			}
		}
		return true;
	}

	ResourceBoundle& operator+= (const ResourceBoundle& other) {
		for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
			(*this)[i] += other.get(i);
		}
		return *this;
	}
	ResourceBoundle& operator-= (const ResourceBoundle& other) {
		for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
			(*this)[i] -= other.get(i);
		}
		return *this;
	}
	
	ResourceBoundle& operator*= (float a) {
		for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
			(*this)[i] *= a;
		}
		return *this;
	}
	
	bool deduct(const ResourceBoundle& amount) {
		if(*this >= amount) {
			*this -= amount;
			return true;
		}
		return false;
	}

private:
	float value[(int) game_resource::RESOURCE_TYPE_COUNT];
};

} // namespace openage

namespace std {

/**
 * hasher for game resource
 */
template<> struct hash<openage::game_resource> {
	typedef underlying_type<openage::game_resource>::type underlying_type;
	typedef hash<underlying_type>::result_type result_type;
	result_type operator()( const openage::game_resource& arg ) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std
