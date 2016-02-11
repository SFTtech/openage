// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace openage {

class UnitType;

/**
 * the key type mapped to data objects
 */
using index_t = int;

/**
 * could use unique ptr
 */
using unit_type_list = std::vector<std::shared_ptr<UnitType>>;

/**
 * contains the initial tech structure for
 * one civilisation
 */
class Civilisation {
public:
	Civilisation(const std::string &name);

private:
	const std::string civ_name;

};

}
