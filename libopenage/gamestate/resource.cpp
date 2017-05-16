// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <string>
#include <cmath>
#include <string>

#include "resource.h"

namespace openage {

ResourceBundle::ResourceBundle()
	:
	value{0} {
}

bool ResourceBundle::operator> (const ResourceBundle& other) const {
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
		if (!(this->get(i) > other.get(i))) {
			return false;
		}
	}
	return true;
}

bool ResourceBundle::operator>= (const ResourceBundle& other) const {
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
		if (!(this->get(i) >= other.get(i))) {
			return false;
		}
	}
	return true;
}

ResourceBundle& ResourceBundle::operator+= (const ResourceBundle& other) {
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
		(*this)[i] += other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator-= (const ResourceBundle& other) {
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
		(*this)[i] -= other.get(i);
	}
	return *this;
}

ResourceBundle& ResourceBundle::operator*= (const double a) {
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
		(*this)[i] *= a;
	}
	return *this;
}

ResourceBundle& ResourceBundle::round() {
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
		(*this)[i] = std::round(this->get(i));
	}
	return *this;
}

bool ResourceBundle::has(const ResourceBundle& amount) const {
	return *this >= amount;
}

bool ResourceBundle::deduct(const ResourceBundle& amount) {
	if (this->has(amount)) {
		*this -= amount;
		return true;
	}
	return false;
}

double ResourceBundle::sum() const {
	double sum = 0;
	for (int i = 0; i < static_cast<int>(game_resource::RESOURCE_TYPE_COUNT); i++) {
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
