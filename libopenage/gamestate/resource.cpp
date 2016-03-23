// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "resource.h"
#include <iostream>
namespace openage {

std::string Resource::getResourceString(game_resource e) {
	switch(e)
	{
		case game_resource::food:  return "food";
		case game_resource::wood:  return "wood";
		case game_resource::gold:  return "gold";
		case game_resource::stone: return "stone";
		default: return("unknown");
	}
}

game_resource Resource::getResourceEnum(std::string resource) {
	if( resource.compare("food") == 0) {
		return game_resource::food;
	}
	if( resource.compare("wood") == 0) {
		 return game_resource::wood;
	}
	if( resource.compare("gold") == 0) {
		  return game_resource::gold;
	}
	return game_resource::stone;
}

} // openage
