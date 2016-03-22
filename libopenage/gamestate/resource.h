// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <string>

namespace openage {

/**
 * probably becomes part of the nyan game spec in future
 */
enum class game_resource {
	wood,
	food,
	gold,
	stone
};

class Resource {
public:
	static std::string getResourceString(game_resource e);
	static game_resource getResourceEnum(std::string resource);
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
