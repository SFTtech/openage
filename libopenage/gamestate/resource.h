// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

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
 * All the resources
 */
class Resources {
public:

	Resources();

	virtual unsigned int get_count() const = 0;

	virtual const Resource& get_resource(int id) const = 0;

	ResourceBundle create_bundle() const;

};

class ClassicResources : public Resources {
public:

	ClassicResources()
		:
		wood{0, "wood"},
		food{1, "food"},
		gold{2, "gold"},
		stone{3, "stone"},
		resources{{0, "wood"}, {1, "food"}, {2, "gold"}, {3, "stone"}}{
	}

	unsigned int get_count() const override { return 4; }

	const Resource& get_resource(int id) const override { return this->resources[id]; };

private:

	const ResourceProducer wood;
	const ResourceProducer food;
	const ResourceProducer gold;
	const ResourceProducer stone;

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
 * Can be also used to store other information about the resources
 */
class ResourceBundle {
public:

	// TODO remove, here for backwards compatibility
	ResourceBundle();

	ResourceBundle(const Resources& resources);

	virtual ~ResourceBundle();

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
	double& operator[] (const int id) { return value[id]; }

	// Getters

	double get(const game_resource res) const { return value[static_cast<int>(res)]; }
	double get(const int id) const { return value[id]; }

	/**
	 * Returns the sum of all the resources.
	 */
	double sum() const;

private:

	int count;
	double *value;

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
