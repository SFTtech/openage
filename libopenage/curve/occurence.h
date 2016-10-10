// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {
namespace curve {

class Occurence
{
public:
	enum class kind
	{
		ATTACK,
		BUILD,
		DIE,
		GARRISON,
		GATHER,
		HEAL,
		MOVE,
		PRODUCE,
		SPAWN,
	};
};

}} // namespace openage::curve
