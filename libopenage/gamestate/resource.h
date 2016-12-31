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
 * a set of amounts of game resources
 */
class ResourceBundle {
public:

	ResourceBundle();

	bool operator> (const ResourceBundle& other) const;
	bool operator>= (const ResourceBundle& other) const;

	ResourceBundle& operator+= (const ResourceBundle& other);
	ResourceBundle& operator-= (const ResourceBundle& other);

	ResourceBundle& operator*= (double a);

	bool has(const ResourceBundle& amount) const;

	bool deduct(const ResourceBundle& amount);

	double& operator[] (game_resource res) { return value[(int) res]; }
	double& operator[] (int index) { return value[index]; }

	// Getters

	double get(game_resource res) const { return value[(int) res]; }
	double get(int index) const { return value[index]; }

private:
	double value[(int) game_resource::RESOURCE_TYPE_COUNT];
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
