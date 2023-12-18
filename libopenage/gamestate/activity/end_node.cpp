// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "end_node.h"

#include "error/error.h"
#include "log/message.h"


namespace openage::gamestate::activity {

EndNode::EndNode(node_id_t id,
                 node_label_t label) :
	Node{id, label, {}} {
}

} // namespace openage::gamestate::activity
