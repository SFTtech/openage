// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <string>
#include <cmath>
#include <string>

#include "resource.h"

namespace openage {

ResourceBundle Resources::create_bundle() const {
	return ResourceBundle(*this);
}

ResourceBundle::ResourceBundle()
	:
	count{4},
	value{new double[count] {0}} {
}

ResourceBundle::ResourceBundle(const Resources& resources)
	:
	count{static_cast<int>(resources.get_count())},
	value{new double[count] {0}} {
}

ResourceBundle::~ResourceBundle() {
	delete[] value;
}

bool ResourceBundle::operator> (const ResourceBundle& other) const {
	for (int i = 0; i < this->count; i++) {
		if (!(this->get(i) > other.get(i))) {
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
	return true;
}

ResourceBundle& ResourceBundle::operator+= (const ResourceBundle& other) {
	for (int i = 0; i < this->count; i++) {
		(*this)[i] += other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator-= (const ResourceBundle& other) {
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
	return true;
}

bool ResourceBundle::deduct(const ResourceBundle& amount) {
	if (this->has(amount)) {
		*this -= amount;
		return true;
	}
	return false;
}

void ResourceBundle::set_all(const double amount) {
	for (int i = 0; i < this->count; i++) {
		(*this)[i] = amount;
	}
}

void ResourceBundle::limit(const ResourceBundle &limits) {
	for (int i = 0; i < this->count; i++) {
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
