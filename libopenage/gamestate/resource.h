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

/**
 *
 */
class ResourceBoundle {
public:

	ResourceBoundle();

	float& operator[] (game_resource res) { return value[(int) res]; }
	float& operator[] (int index) { return value[index]; }

	float get(game_resource res) const { return value[(int) res]; }
	float get(int index) const { return value[index]; }

	bool operator> (const ResourceBoundle& other) const;
	bool operator>= (const ResourceBoundle& other) const;

	ResourceBoundle& operator+= (const ResourceBoundle& other);
	ResourceBoundle& operator-= (const ResourceBoundle& other);

	ResourceBoundle& operator*= (float a);

	bool has(const ResourceBoundle& amount) const;

	bool deduct(const ResourceBoundle& amount);

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
