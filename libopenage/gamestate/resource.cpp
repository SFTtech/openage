// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cmath>

#include "resource.h"

namespace openage {

ResourceBundle::ResourceBundle()
	:
	value{0} {
}

bool ResourceBundle::operator> (const ResourceBundle& other) const {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		if (!(this->get(i) > other.get(i))) {
			return false;
		}
	}
	return true;
}

bool ResourceBundle::operator>= (const ResourceBundle& other) const {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		if (!(this->get(i) >= other.get(i))) {
			return false;
		}
	}
	return true;
}

ResourceBundle& ResourceBundle::operator+= (const ResourceBundle& other) {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] += other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator-= (const ResourceBundle& other) {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] -= other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator*= (const double a) {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] *= a;
	}
	return *this;
}

ResourceBundle& ResourceBundle::round() {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] = std::round(this->get(i));
	}
	return *this;
}

bool ResourceBundle::has(const ResourceBundle& amount) const {
	return *this >= amount;
}

bool ResourceBundle::deduct(const ResourceBundle& amount) {
	if (*this >= amount) {
		*this -= amount;
		return true;
	}
	return false;
}

} // openage
