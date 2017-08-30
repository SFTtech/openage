// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

namespace openage {

/**
 * probably becomes part of the nyan game spec in future
 */
enum class game_resource : int {
	wood,
	food,
	gold,
	stone,
	RESOURCE_TYPE_COUNT
};

/**
 * A set of amounts of game resources.
 *
 * Can be also used to store other information about the resources
 */
class ResourceBundle {
public:

	ResourceBundle();

	bool operator> (const ResourceBundle& other) const;
	bool operator>= (const ResourceBundle& other) const;

	ResourceBundle& operator+= (const ResourceBundle& other);
	ResourceBundle& operator-= (const ResourceBundle& other);

	ResourceBundle& operator*= (const double a);

	/**
	 * Round each value to the nearest integer.
	 * Returns itself.
	 */
	ResourceBundle& round();

	bool has(const ResourceBundle& amount) const;

	bool has(const ResourceBundle& amount1, const ResourceBundle& amount2) const;

	/**
	 * If amount can't be deducted return false, else deduct the given amount
	 * and return true.
	 */
	bool deduct(const ResourceBundle& amount);

	void set_all(const double amount);

	void limit(const ResourceBundle& limits);

	double& operator[] (const game_resource res) { return value[static_cast<int>(res)]; }
	double& operator[] (const int index) { return value[index]; }

	// Getters

	double get(const game_resource res) const { return value[static_cast<int>(res)]; }
	double get(const int index) const { return value[index]; }

	/**
	 * Returns the sum of all the resources.
	 */
	double sum() const;

private:

	double value[static_cast<int>(game_resource::RESOURCE_TYPE_COUNT)];

};

} // namespace openage

namespace std {

std::string to_string(const openage::game_resource &res);

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
