// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include <string>
#include <cmath>
#include <string>

#include "resource.h"

namespace openage {

ClassicResources::ClassicResources()
	:
	resources{{wood, "wood"},
	          {food, "food"},
	          {gold, "gold"},
	          {stone, "stone"}} {
}

// TODO remove, here for transition period
const Resource* ClassicResources::to_resource(game_resource& id) {
	static ClassicResources cr = ClassicResources();
	return &cr.get_resource((int) id);
}

ResourceBundle Resources::create_bundle() const {
	return ResourceBundle(*this);
}

ResourceBundle::ResourceBundle()
	:
	ResourceBundle{4} {
}

ResourceBundle::ResourceBundle(const Resources& resources)
	:
	ResourceBundle{static_cast<int>(resources.get_count())} {
}

ResourceBundle::ResourceBundle(const int count)
	:
	count{count},
	value{new double[count] {0}} {
}

ResourceBundle::ResourceBundle(const ResourceBundle &resources)
	:
	ResourceBundle{resources.count} {
	this->set(resources);
}

ResourceBundle ResourceBundle::clone() const {
	return ResourceBundle(*this);
}

ResourceBundle::~ResourceBundle() {
	delete[] value;
}

void ResourceBundle::expand(const ResourceBundle& other) {
	this->expand(other.count);
}

void ResourceBundle::expand(const int count) {
	if (this->count == count) {
		return;
	}
	// create new array with old values
	double *new_value = new double[count];
	for (int i = 0; i < this->count; i++) {
		new_value[i] = this->value[i];
	}
	// replace the private variables
	this->count = count;
	delete[] value;
	this->value = new_value;
}

bool ResourceBundle::operator> (const ResourceBundle& other) const {
	for (int i = 0; i < this->count; i++) {
		if (!(this->get(i) > other.get(i))) {
			return false;
		}
	}
	// check also resources that are not in both bundles
	for (int i = this->count; i < other.count; i++) {
		if (other.get(i) > 0) {
			return false;
		}
	}
	return true;
}

bool ResourceBundle::operator>= (const ResourceBundle& other) const {
	for (int i = 0; i < this->count; i++) {
		if (!(this->get(i) >= other.get(i))) {
			return false;
		}
	}
	// check also resources that are not in both bundles
	for (int i = this->count; i < other.count; i++) {
		if (other.get(i) > 0) {
			return false;
		}
	}
	return true;
}

ResourceBundle& ResourceBundle::operator+= (const ResourceBundle& other) {
	this->expand(other);
	for (int i = 0; i < this->count; i++) {
		(*this)[i] += other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator-= (const ResourceBundle& other) {
	this->expand(other);
	for (int i = 0; i < this->count; i++) {
		(*this)[i] -= other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator*= (const double a) {
	for (int i = 0; i < this->count; i++) {
		(*this)[i] *= a;
	}
	return *this;
}

ResourceBundle& ResourceBundle::round() {
	for (int i = 0; i < this->count; i++) {
		(*this)[i] = std::round(this->get(i));
	}
	return *this;
}

bool ResourceBundle::has(const ResourceBundle& amount) const {
	return *this >= amount;
}

bool ResourceBundle::has(const ResourceBundle& amount1, const ResourceBundle& amount2) const {
	for (int i = 0; i < this->count; i++) {
		if (!(this->get(i) >= amount1.get(i) + amount2.get(i))) {
			return false;
		}
	}
	// check also resources that are not in both bundles
	for (int i = this->count; i < amount1.count; i++) {
		if (amount1.get(i) > 0) {
			return false;
		}
	}
	for (int i = this->count; i < amount2.count; i++) {
		if (amount2.get(i) > 0) {
			return false;
		}
	}
	return true;
}

bool ResourceBundle::deduct(const ResourceBundle& amount) {
	if (this->has(amount)) {
		*this -= amount;
		return true;
	}
	return false;
}

void ResourceBundle::set(const ResourceBundle &amount) {
	this->expand(amount);
	for (int i = 0; i < this->count; i++) {
		(*this)[i] = amount.get(i);
	}
}

void ResourceBundle::set_all(const double amount) {
	for (int i = 0; i < this->count; i++) {
		(*this)[i] = amount;
	}
}

void ResourceBundle::limit(const ResourceBundle &limits) {
	for (int i = 0; i < this->min_count(limits); i++) {
		if (this->get(i) > limits.get(i)) {
			(*this)[i] = limits.get(i);
		}
	}
}

double ResourceBundle::sum() const {
	double sum = 0;
	for (int i = 0; i < this->count; i++) {
		sum += this->get(i);
	}
	return sum;
}

int ResourceBundle::min_count(const ResourceBundle &other) {
	return this->count <= other.count ? this->count : other.count;
}

} // openage

namespace std {

string to_string(const openage::game_resource &res) {
	switch (res) {
	case openage::game_resource::wood:
		return "wood";
	case openage::game_resource::food:
		return "food";
	case openage::game_resource::gold:
		return "gold";
	case openage::game_resource::stone:
		return "stone";
	default:
		return "unknown";
	}
}

} // namespace std
