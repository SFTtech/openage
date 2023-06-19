// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once


namespace openage::gamestate::activity {

class Node {
public:
	Node();
	virtual ~Node();

	virtual void visit() = 0;
};

} // namespace openage::gamestate::activity
