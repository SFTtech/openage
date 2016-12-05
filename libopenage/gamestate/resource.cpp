// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "resource.h"

namespace openage {

ResourceBoundle::ResourceBoundle()
	:
	value{0} {
}

bool ResourceBoundle::operator> (const ResourceBoundle& other) const {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		if (!(this->get(i) > other.get(i))) {
			return false;
		}
	}
	return true;
}

bool ResourceBoundle::operator>= (const ResourceBoundle& other) const {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		if (!(this->get(i) >= other.get(i))) {
			return false;
		}
	}
	return true;
}

ResourceBoundle& ResourceBoundle::operator+= (const ResourceBoundle& other) {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] += other.get(i);
	}
	return *this;
}

ResourceBoundle& ResourceBoundle::operator-= (const ResourceBoundle& other) {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] -= other.get(i);
	}
	return *this;
}

ResourceBoundle& ResourceBoundle::operator*= (float a) {
	for (int i=0; i<(int) game_resource::RESOURCE_TYPE_COUNT; i++) {
		(*this)[i] *= a;
	}
	return *this;
}

bool ResourceBoundle::has(const ResourceBoundle& amount) const {
	return *this >= amount;
}

bool ResourceBoundle::deduct(const ResourceBoundle& amount) {
	if(*this >= amount) {
		*this -= amount;
		return true;
	}
	return false;
}

} // openage
