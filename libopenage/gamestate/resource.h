// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <string>

namespace openage {

class ResourceBundle;

/**
 * A resource
 */
class Resource {
public:

	Resource();

	virtual int id() const = 0;

	virtual std::string name() const = 0;

	// TODO add images and icons

};

class ResourceProducer : public Resource {
public:

	ResourceProducer(int id, std::string name)
		:
		_id{id},
		_name{name} { }

	int id() const override { return _id; }

	std::string name() const override { return _name; }

private:

	int _id;
	std::string _name;
};

/**
 * All the resources.
 *
 * The the ids of the resources must be inside [0, count).
 *
 * The static variables wood, food, gold, stone are the ids of the representing resource.
 * Any extension of Resources must use this ids as they are an engine dependency (at the moment).
 */
class Resources {
public:

	Resources();

	virtual unsigned int get_count() const = 0;

	virtual const Resource& get_resource(int id) const = 0;

	ResourceBundle create_bundle() const;

	// TODO remove when the engine is fully decupled from the data
	static const int wood = 0;
	static const int food = 1;
	static const int gold = 2;
	static const int stone = 3;

};

class ClassicResources : public Resources {
public:

	ClassicResources()
		:
		resources{{Resources::wood, "wood"},
		          {Resources::food, "food"},
		          {Resources::gold, "gold"},
		          {Resources::stone, "stone"}} {
	}

	unsigned int get_count() const override { return 4; }

	const Resource& get_resource(int id) const override { return this->resources[id]; };

private:

	const ResourceProducer resources[4];
};

// TODO remove, here for backwards compatibility
enum class game_resource : int {
	wood = 0,
	food = 1,
	gold = 2,
	stone = 3,
	RESOURCE_TYPE_COUNT = 4
};


/**
 * A set of amounts of game resources.
 *
 * Can be also used to store other information about the resources.
 *
 * TODO change amounts from doubles to integers
 */
class ResourceBundle {
public:

	// TODO remove, here for backwards compatibility
	ResourceBundle();

	ResourceBundle(const Resources& resources);

	virtual ~ResourceBundle();

	ResourceBundle(const ResourceBundle& other);

	ResourceBundle clone() const;

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

	void set(const ResourceBundle& amount);

	void set_all(const double amount);

	void limit(const ResourceBundle& limits);

	double& operator[] (const game_resource res) { return value[static_cast<int>(res)]; }
	double& operator[] (const int id) { return value[id]; }

	// Getters

	double get(const game_resource res) const { return value[static_cast<int>(res)]; }
	double get(const int id) const { return value[id]; }

	/**
	 * Returns the sum of all the resources.
	 */
	double sum() const;

	/**
	 * The number of resources
	 */
	int get_count() const { return count; };

private:

	ResourceBundle(const int count);

	void expand(const ResourceBundle& other);

	void expand(const int count);

	/**
	 * The number of resources
	 */
	int count;

	double *value;

	int min_count(const ResourceBundle& other);

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
